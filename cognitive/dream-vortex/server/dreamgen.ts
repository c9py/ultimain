import { getApiKeyById, updateApiKeyLastUsed } from "./db";

const DREAMGEN_BASE_URL = "https://dreamgen.com/api";

export interface SamplingParams {
  temperature?: number;
  topP?: number;
  topK?: number;
  minP?: number;
  maxTokens?: number;
  presencePenalty?: number;
  frequencyPenalty?: number;
  repetitionPenalty?: number;
  stopSequences?: string[];
}

export interface ChatMessage {
  role: "system" | "user" | "assistant" | "text";
  name?: string;
  content: string;
}

export interface RoleConfig {
  assistant: {
    role: "assistant" | "text";
    name?: string;
    open?: boolean;
  };
  user?: {
    role: "user" | "text";
    name?: string;
  };
}

// Simple encryption for API keys (in production, use proper encryption)
const ENCRYPTION_KEY = process.env.JWT_SECRET || "default-key";

export function encryptApiKey(apiKey: string): string {
  // Simple XOR encryption - in production use proper encryption like AES
  const keyBytes = Buffer.from(ENCRYPTION_KEY);
  const inputBytes = Buffer.from(apiKey);
  const encrypted = Buffer.alloc(inputBytes.length);
  
  for (let i = 0; i < inputBytes.length; i++) {
    encrypted[i] = inputBytes[i] ^ keyBytes[i % keyBytes.length];
  }
  
  return encrypted.toString("base64");
}

export function decryptApiKey(encryptedKey: string): string {
  const keyBytes = Buffer.from(ENCRYPTION_KEY);
  const inputBytes = Buffer.from(encryptedKey, "base64");
  const decrypted = Buffer.alloc(inputBytes.length);
  
  for (let i = 0; i < inputBytes.length; i++) {
    decrypted[i] = inputBytes[i] ^ keyBytes[i % keyBytes.length];
  }
  
  return decrypted.toString();
}

export async function getDecryptedApiKey(keyId: number, userId: number): Promise<string | null> {
  const apiKey = await getApiKeyById(keyId, userId);
  if (!apiKey) return null;
  
  await updateApiKeyLastUsed(keyId);
  return decryptApiKey(apiKey.encryptedKey);
}

// Chat completion using OpenAI-compatible endpoint
export async function chatCompletion(
  apiKey: string,
  modelId: string,
  messages: ChatMessage[],
  roleConfig?: RoleConfig,
  samplingParams?: SamplingParams,
  stream = true
): Promise<Response> {
  const url = `${DREAMGEN_BASE_URL}/openai/v1/chat/completions`;
  
  const body: Record<string, unknown> = {
    model: modelId,
    messages: messages.map(m => ({
      role: m.role === "text" ? "assistant" : m.role,
      name: m.name,
      content: m.content,
    })),
    stream,
  };
  
  if (roleConfig) {
    body.role_config = roleConfig;
  }
  
  if (samplingParams) {
    if (samplingParams.temperature !== undefined) body.temperature = samplingParams.temperature;
    if (samplingParams.topP !== undefined) body.top_p = samplingParams.topP;
    if (samplingParams.maxTokens !== undefined) body.max_tokens = samplingParams.maxTokens;
    if (samplingParams.presencePenalty !== undefined) body.presence_penalty = samplingParams.presencePenalty;
    if (samplingParams.frequencyPenalty !== undefined) body.frequency_penalty = samplingParams.frequencyPenalty;
    if (samplingParams.stopSequences !== undefined) body.stop = samplingParams.stopSequences;
  }
  
  const response = await fetch(url, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
      "Authorization": `Bearer ${apiKey}`,
    },
    body: JSON.stringify(body),
  });
  
  return response;
}

// Text completion using native DreamGen API
export async function textCompletion(
  apiKey: string,
  modelId: string,
  input: string,
  samplingParams?: SamplingParams
): Promise<Response> {
  const url = `${DREAMGEN_BASE_URL}/v1/model/completion`;
  
  const body = {
    modelId,
    input,
    samplingParams: {
      kind: "basic" as const,
      ...samplingParams,
    },
  };
  
  const response = await fetch(url, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
      "Authorization": `Bearer ${apiKey}`,
    },
    body: JSON.stringify(body),
  });
  
  return response;
}

// Build story prompt in DreamGen format
export function buildStoryPrompt(
  plotDescription: string,
  characters: Array<{ name: string; description: string }>,
  styleDescription?: string,
  previousContent?: string
): string {
  let prompt = `<|im_start|>system
You are an intelligent, skilled, versatile writer.

Your task is to write a story based on the information below.

## Overall plot description:
${plotDescription}
`;

  if (styleDescription) {
    prompt += `
## Style description:
${styleDescription}
`;
  }

  if (characters.length > 0) {
    prompt += `
## Characters:
`;
    for (const char of characters) {
      prompt += `
### ${char.name}
${char.description}
`;
    }
  }

  prompt += `<|im_end|>`;

  if (previousContent) {
    prompt += `
<|im_start|>text
${previousContent}<|im_end|>`;
  }

  prompt += `
<|im_start|>text
`;

  return prompt;
}

// Build role-play prompt
export function buildRolePlayPrompt(
  scenarioDescription: string,
  characters: Array<{ name: string; label: string; description: string; isUserCharacter?: boolean }>,
  interactions: Array<{ type: "message" | "text" | "instruction"; characterLabel?: string; content: string }>
): string {
  let prompt = `<|im_start|>system
You are an intelligent, skilled, versatile writer.

Your task is to role-play based on the scenario below.

## Scenario:
${scenarioDescription}

## Characters:
`;

  for (const char of characters) {
    prompt += `
### ${char.name}
${char.description}
`;
  }

  prompt += `<|im_end|>`;

  // Add interactions
  for (const interaction of interactions) {
    if (interaction.type === "message" && interaction.characterLabel) {
      const character = characters.find(c => c.label === interaction.characterLabel);
      const charName = character?.name || interaction.characterLabel;
      prompt += `
<|im_start|>text character: ${charName}
${interaction.content}<|im_end|>`;
    } else if (interaction.type === "text") {
      prompt += `
<|im_start|>text
${interaction.content}<|im_end|>`;
    } else if (interaction.type === "instruction") {
      prompt += `
<|im_start|>user
${interaction.content}<|im_end|>`;
    }
  }

  return prompt;
}

// Available models
export const AVAILABLE_MODELS = [
  { id: "lucid-v1-medium", name: "Lucid V1 Medium", size: "sm" },
  { id: "lucid-v1-extra-large", name: "Lucid V1 Extra Large", size: "xl" },
] as const;

// Default sampling parameters
export const DEFAULT_SAMPLING_PARAMS: SamplingParams = {
  temperature: 0.8,
  topP: 0.95,
  topK: 50,
  minP: 0.05,
  maxTokens: 500,
  presencePenalty: 0,
  frequencyPenalty: 0,
  repetitionPenalty: 1.0,
};
