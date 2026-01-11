import DashboardLayout from "@/components/DashboardLayout";
import { Button } from "@/components/ui/button";
import { Card } from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { Textarea } from "@/components/ui/textarea";
import { trpc } from "@/lib/trpc";
import { ArrowLeft, Loader2, MessageSquare, Send, Settings, User, BookOpen, Wand2 } from "lucide-react";
import { useEffect, useRef, useState } from "react";
import { Link, useParams } from "wouter";
import { toast } from "sonner";
import { Streamdown } from "streamdown";

type MessageType = "message" | "text" | "instruction" | "user";

interface LocalMessage {
  id?: number;
  messageType: MessageType;
  characterLabel?: string;
  characterName?: string;
  content: string;
  isStreaming?: boolean;
}

export default function ChatSession() {
  const params = useParams<{ id: string }>();
  const sessionId = parseInt(params.id || "0");
  
  const { data: session, isLoading, refetch } = trpc.chat.sessionById.useQuery({ id: sessionId });
  const { data: apiKeys } = trpc.apiKeys.list.useQuery();
  const addMessageMutation = trpc.chat.addMessage.useMutation();
  const updateSessionMutation = trpc.chat.updateSession.useMutation();

  const [messages, setMessages] = useState<LocalMessage[]>([]);
  const [inputValue, setInputValue] = useState("");
  const [messageType, setMessageType] = useState<MessageType>("user");
  const [characterName, setCharacterName] = useState("");
  const [isGenerating, setIsGenerating] = useState(false);
  const [showSettings, setShowSettings] = useState(false);
  
  // Sampling params
  const [temperature, setTemperature] = useState(0.8);
  const [topP, setTopP] = useState(0.95);
  const [maxTokens, setMaxTokens] = useState(500);
  const [modelId, setModelId] = useState("lucid-v1-medium");

  const messagesEndRef = useRef<HTMLDivElement>(null);
  const hasApiKey = apiKeys && apiKeys.length > 0;

  useEffect(() => {
    if (session) {
      // Messages are loaded separately
      setMessages(((session as any).messages || []).map((m: any) => ({
        id: m.id,
        messageType: m.messageType as MessageType,
        characterLabel: m.characterLabel ?? undefined,
        characterName: m.characterName ?? undefined,
        content: m.content,
      })));
      setModelId(session.modelId || "lucid-v1-medium");
      if (session.samplingParams) {
        const params = session.samplingParams as Record<string, number>;
        if (params.temperature) setTemperature(params.temperature);
        if (params.topP) setTopP(params.topP);
        if (params.maxTokens) setMaxTokens(params.maxTokens);
      }
    }
  }, [session]);

  useEffect(() => {
    messagesEndRef.current?.scrollIntoView({ behavior: "smooth" });
  }, [messages]);

  const handleSend = async () => {
    if (!inputValue.trim() || !hasApiKey) return;

    const newMessage: LocalMessage = {
      messageType,
      characterName: messageType === "message" ? characterName : undefined,
      content: inputValue.trim(),
    };

    setMessages(prev => [...prev, newMessage]);
    setInputValue("");

    // Save message to database
    try {
      await addMessageMutation.mutateAsync({
        sessionId,
        messageType,
        characterName: messageType === "message" ? characterName : undefined,
        content: newMessage.content,
      });
    } catch (error) {
      toast.error("Failed to save message");
    }

    // Generate AI response
    if (messageType === "user" || messageType === "instruction") {
      await generateResponse();
    }
  };

  const generateResponse = async () => {
    if (!apiKeys || apiKeys.length === 0) {
      toast.error("No API key configured");
      return;
    }

    setIsGenerating(true);
    
    // Add placeholder for streaming response
    const streamingMessage: LocalMessage = {
      messageType: "text",
      content: "",
      isStreaming: true,
    };
    setMessages(prev => [...prev, streamingMessage]);

    try {
      // Build messages for API
      const apiMessages = messages.map(m => ({
        role: m.messageType === "user" || m.messageType === "instruction" ? "user" as const : 
              m.messageType === "message" ? "assistant" as const : "assistant" as const,
        name: m.characterName,
        content: m.content,
      }));

      // Add system prompt if exists
      if (session?.systemPrompt) {
        apiMessages.unshift({
          role: "user" as const,
          name: undefined,
          content: session.systemPrompt,
        });
      }

      // For now, simulate streaming with a placeholder
      // In production, this would call the actual DreamGen API
      const response = "This is a simulated AI response. To enable real generation, the DreamGen API integration needs to be connected with your API key. The system supports streaming responses with markdown rendering.";
      
      let currentContent = "";
      for (let i = 0; i < response.length; i++) {
        await new Promise(resolve => setTimeout(resolve, 20));
        currentContent += response[i];
        setMessages(prev => {
          const newMessages = [...prev];
          const lastIndex = newMessages.length - 1;
          if (newMessages[lastIndex]?.isStreaming) {
            newMessages[lastIndex] = { ...newMessages[lastIndex], content: currentContent };
          }
          return newMessages;
        });
      }

      // Finalize message
      setMessages(prev => {
        const newMessages = [...prev];
        const lastIndex = newMessages.length - 1;
        if (newMessages[lastIndex]?.isStreaming) {
          newMessages[lastIndex] = { ...newMessages[lastIndex], isStreaming: false };
        }
        return newMessages;
      });

      // Save to database
      await addMessageMutation.mutateAsync({
        sessionId,
        messageType: "text",
        content: response,
      });

    } catch (error) {
      toast.error("Failed to generate response");
      setMessages(prev => prev.filter(m => !m.isStreaming));
    } finally {
      setIsGenerating(false);
    }
  };

  const handleSaveSettings = async () => {
    try {
      await updateSessionMutation.mutateAsync({
        id: sessionId,
        title: session?.title || "Chat Session",
      });
      toast.success("Settings saved");
      setShowSettings(false);
    } catch (error) {
      toast.error("Failed to save settings");
    }
  };

  const getMessageStyle = (type: MessageType) => {
    switch (type) {
      case "message":
        return "message-character bg-card";
      case "text":
        return "message-narrator bg-card";
      case "instruction":
        return "message-instruction bg-card";
      case "user":
        return "bg-primary/10 border-l-3 border-primary";
      default:
        return "bg-card";
    }
  };

  const getMessageIcon = (type: MessageType) => {
    switch (type) {
      case "message":
        return <User className="h-4 w-4" />;
      case "text":
        return <BookOpen className="h-4 w-4" />;
      case "instruction":
        return <Wand2 className="h-4 w-4" />;
      case "user":
        return <MessageSquare className="h-4 w-4" />;
      default:
        return null;
    }
  };

  if (isLoading) {
    return (
      <DashboardLayout>
        <div className="flex items-center justify-center h-96">
          <Loader2 className="h-8 w-8 animate-spin text-muted-foreground" />
        </div>
      </DashboardLayout>
    );
  }

  if (!session) {
    return (
      <DashboardLayout>
        <div className="text-center py-12">
          <p className="text-muted-foreground">Session not found</p>
          <Link href="/chat">
            <Button variant="link">Back to Chat</Button>
          </Link>
        </div>
      </DashboardLayout>
    );
  }

  return (
    <DashboardLayout>
      <div className="flex flex-col h-[calc(100vh-8rem)]">
        {/* Header */}
        <div className="flex items-center justify-between mb-4">
          <div className="flex items-center gap-4">
            <Link href="/chat">
              <Button variant="ghost" size="sm">
                <ArrowLeft className="h-4 w-4 mr-2" />
                Back
              </Button>
            </Link>
            <h1 className="text-xl font-bold">{session.title}</h1>
          </div>
          <Button variant="outline" size="sm" onClick={() => setShowSettings(!showSettings)}>
            <Settings className="h-4 w-4 mr-2" />
            Settings
          </Button>
        </div>

        {/* Settings Panel */}
        {showSettings && (
          <Card className="p-4 mb-4">
            <div className="grid md:grid-cols-4 gap-4">
              <div className="space-y-2">
                <Label>Model</Label>
                <Select value={modelId} onValueChange={setModelId}>
                  <SelectTrigger>
                    <SelectValue />
                  </SelectTrigger>
                  <SelectContent>
                    <SelectItem value="lucid-v1-medium">Lucid V1 Medium</SelectItem>
                    <SelectItem value="lucid-v1-extra-large">Lucid V1 Extra Large</SelectItem>
                  </SelectContent>
                </Select>
              </div>
              <div className="space-y-2">
                <Label>Temperature: {temperature}</Label>
                <Input
                  type="range"
                  min="0"
                  max="2"
                  step="0.1"
                  value={temperature}
                  onChange={(e) => setTemperature(parseFloat(e.target.value))}
                />
              </div>
              <div className="space-y-2">
                <Label>Top P: {topP}</Label>
                <Input
                  type="range"
                  min="0"
                  max="1"
                  step="0.05"
                  value={topP}
                  onChange={(e) => setTopP(parseFloat(e.target.value))}
                />
              </div>
              <div className="space-y-2">
                <Label>Max Tokens: {maxTokens}</Label>
                <Input
                  type="number"
                  min="1"
                  max="4096"
                  value={maxTokens}
                  onChange={(e) => setMaxTokens(parseInt(e.target.value))}
                />
              </div>
            </div>
            <div className="mt-4 flex justify-end">
              <Button size="sm" onClick={handleSaveSettings}>
                Save Settings
              </Button>
            </div>
          </Card>
        )}

        {/* Messages */}
        <div className="flex-1 overflow-y-auto space-y-4 mb-4">
          {messages.length === 0 ? (
            <div className="text-center py-12 text-muted-foreground">
              <MessageSquare className="h-12 w-12 mx-auto mb-4 opacity-50" />
              <p>No messages yet. Start the conversation!</p>
            </div>
          ) : (
            messages.map((message, index) => (
              <div
                key={index}
                className={`p-4 rounded-lg ${getMessageStyle(message.messageType)}`}
              >
                <div className="flex items-center gap-2 mb-2 text-sm text-muted-foreground">
                  {getMessageIcon(message.messageType)}
                  <span className="capitalize">
                    {message.messageType === "message" && message.characterName
                      ? message.characterName
                      : message.messageType}
                  </span>
                </div>
                <div className="prose prose-sm dark:prose-invert max-w-none">
                  <Streamdown>{message.content}</Streamdown>
                  {message.isStreaming && (
                    <span className="inline-block w-2 h-4 bg-primary animate-pulse ml-1" />
                  )}
                </div>
              </div>
            ))
          )}
          <div ref={messagesEndRef} />
        </div>

        {/* Input Area */}
        <div className="border-t pt-4">
          <div className="flex gap-2 mb-2">
            <Tabs value={messageType} onValueChange={(v) => setMessageType(v as MessageType)} className="flex-1">
              <TabsList className="grid grid-cols-4 w-full max-w-md">
                <TabsTrigger value="user">User</TabsTrigger>
                <TabsTrigger value="message">Character</TabsTrigger>
                <TabsTrigger value="text">Narrator</TabsTrigger>
                <TabsTrigger value="instruction">Instruction</TabsTrigger>
              </TabsList>
            </Tabs>
          </div>
          
          {messageType === "message" && (
            <div className="mb-2">
              <Input
                placeholder="Character name..."
                value={characterName}
                onChange={(e) => setCharacterName(e.target.value)}
                className="max-w-xs"
              />
            </div>
          )}

          <div className="flex gap-2">
            <Textarea
              placeholder={
                messageType === "user" ? "Type your message..." :
                messageType === "message" ? "Write as character..." :
                messageType === "text" ? "Write narrator text..." :
                "Write an instruction for the AI..."
              }
              value={inputValue}
              onChange={(e) => setInputValue(e.target.value)}
              onKeyDown={(e) => {
                if (e.key === "Enter" && !e.shiftKey) {
                  e.preventDefault();
                  handleSend();
                }
              }}
              className="flex-1 min-h-[80px]"
              disabled={!hasApiKey || isGenerating}
            />
            <Button
              onClick={handleSend}
              disabled={!inputValue.trim() || !hasApiKey || isGenerating}
              className="self-end"
            >
              {isGenerating ? (
                <Loader2 className="h-4 w-4 animate-spin" />
              ) : (
                <Send className="h-4 w-4" />
              )}
            </Button>
          </div>
          
          {!hasApiKey && (
            <p className="text-sm text-muted-foreground mt-2">
              Configure your API key to start chatting
            </p>
          )}
        </div>
      </div>
    </DashboardLayout>
  );
}
