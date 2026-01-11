import DashboardLayout from "@/components/DashboardLayout";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Checkbox } from "@/components/ui/checkbox";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select";
import { Textarea } from "@/components/ui/textarea";
import { trpc } from "@/lib/trpc";
import { ArrowLeft, Loader2, MessageSquare, Plus, Save, Trash2, User, Wand2, BookOpen } from "lucide-react";
import { useEffect, useState } from "react";
import { Link, useLocation, useParams } from "wouter";
import { toast } from "sonner";

interface ScenarioCharacter {
  id?: number;
  name: string;
  label: string;
  promptDescription: string;
  isUserCharacter: boolean;
}

interface ScenarioInteraction {
  id?: number;
  interactionType: "message" | "text" | "instruction";
  characterLabel?: string;
  content: string;
  isSticky: boolean;
}

export default function ScenarioEditor() {
  const params = useParams<{ id: string }>();
  const [, setLocation] = useLocation();
  const isNew = params.id === "new" || !params.id;
  const scenarioId = isNew ? 0 : parseInt(params.id || "0");
  
  const { data: scenario, isLoading } = trpc.scenarios.byId.useQuery(
    { id: scenarioId },
    { enabled: !isNew && scenarioId > 0 }
  );
  const createMutation = trpc.scenarios.create.useMutation();
  const updateMutation = trpc.scenarios.update.useMutation();
  // Character and interaction mutations
  const addCharacterMutation = trpc.scenarios.addCharacter.useMutation();
  const updateCharacterMutation = trpc.scenarios.updateCharacter.useMutation();
  const removeCharacterMutation = trpc.scenarios.removeCharacter.useMutation();
  const addInteractionMutation = trpc.scenarios.addInteraction.useMutation();
  const updateInteractionMutation = trpc.scenarios.updateInteraction.useMutation();
  const removeInteractionMutation = trpc.scenarios.removeInteraction.useMutation();

  const [title, setTitle] = useState("");
  const [promptDescription, setPromptDescription] = useState("");
  const [displayDescription, setDisplayDescription] = useState("");
  const [isPublic, setIsPublic] = useState(false);
  const [characters, setCharacters] = useState<ScenarioCharacter[]>([]);
  const [interactions, setInteractions] = useState<ScenarioInteraction[]>([]);
  const [isSaving, setIsSaving] = useState(false);
  const [currentScenarioId, setCurrentScenarioId] = useState<number | null>(null);

  useEffect(() => {
    if (scenario) {
      setTitle(scenario.title);
      setPromptDescription(scenario.promptDescription || "");
      setDisplayDescription(scenario.displayDescription || "");
      setIsPublic(scenario.isPublic || false);
      setCurrentScenarioId(scenario.id);
      setCharacters(((scenario as any).characters || [])?.map((c: any) => ({
        id: c.id,
        name: c.name,
        label: c.label,
        promptDescription: c.promptDescription || "",
        isUserCharacter: c.isUserCharacter || false,
      })) || []);
      setInteractions(((scenario as any).interactions || [])?.map((i: any) => ({
        id: i.id,
        interactionType: i.interactionType,
        characterLabel: i.characterLabel || undefined,
        content: i.content,
        isSticky: i.isSticky || false,
      })) || []);
    }
  }, [scenario]);

  const handleSave = async () => {
    if (!title.trim()) {
      toast.error("Title is required");
      return;
    }

    setIsSaving(true);
    try {
      if (isNew || !currentScenarioId) {
        const result = await createMutation.mutateAsync({
          title,
          promptDescription,
          displayDescription,
          isPublic,
        });
        setCurrentScenarioId(result as number);
        toast.success("Scenario created");
        setLocation(`/scenarios/${result}`);
      } else {
        await updateMutation.mutateAsync({
          id: currentScenarioId,
          title,
          promptDescription,
          displayDescription,
          isPublic,
        });
        toast.success("Scenario saved");
      }
    } catch (error) {
      toast.error("Failed to save scenario");
    } finally {
      setIsSaving(false);
    }
  };

  const handleAddCharacter = () => {
    setCharacters(prev => [...prev, {
      name: "",
      label: "",
      promptDescription: "",
      isUserCharacter: false,
    }]);
  };

  const handleSaveCharacter = async (index: number) => {
    if (!currentScenarioId) {
      toast.error("Please save the scenario first");
      return;
    }

    const char = characters[index];
    if (!char.name.trim() || !char.label.trim()) {
      toast.error("Character name and label are required");
      return;
    }

    // Validate label format
    if (!/^[a-z0-9_]+$/.test(char.label)) {
      toast.error("Label must be lowercase letters, numbers, and underscores only");
      return;
    }

    try {
      if (!char.id) {
        const result = await addCharacterMutation.mutateAsync({
          scenarioId: currentScenarioId,
          name: char.name,
          label: char.label,
          promptDescription: char.promptDescription,
          isUserCharacter: char.isUserCharacter,
          orderIndex: index,
        });
        setCharacters(prev => {
          const newChars = [...prev];
          newChars[index] = { ...newChars[index], id: result as number };
          return newChars;
        });
      } else {
        await updateCharacterMutation.mutateAsync({
          id: char.id,
          scenarioId: currentScenarioId,
          name: char.name,
          label: char.label,
          promptDescription: char.promptDescription,
          isUserCharacter: char.isUserCharacter,
          orderIndex: index,
        });
      }
      toast.success("Character saved");
    } catch (error) {
      toast.error("Failed to save character");
    }
  };

  const handleRemoveCharacter = async (index: number) => {
    const char = characters[index];
    if (char.id && currentScenarioId) {
      try {
        await removeCharacterMutation.mutateAsync({ id: char.id, scenarioId: currentScenarioId });
      } catch (error) {
        toast.error("Failed to remove character");
        return;
      }
    }
    setCharacters(prev => prev.filter((_, i) => i !== index));
    toast.success("Character removed");
  };

  const handleAddInteraction = (type: "message" | "text" | "instruction") => {
    setInteractions(prev => [...prev, {
      interactionType: type,
      characterLabel: type === "message" ? characters[0]?.label : undefined,
      content: "",
      isSticky: false,
    }]);
  };

  const handleSaveInteraction = async (index: number) => {
    if (!currentScenarioId) {
      toast.error("Please save the scenario first");
      return;
    }

    const interaction = interactions[index];
    if (!interaction.content.trim()) {
      toast.error("Content is required");
      return;
    }

    try {
      if (!interaction.id) {
        const result = await addInteractionMutation.mutateAsync({
          scenarioId: currentScenarioId,
          interactionType: interaction.interactionType,
          characterLabel: interaction.characterLabel,
          content: interaction.content,
          isSticky: interaction.isSticky,
          orderIndex: index,
        });
        setInteractions(prev => {
          const newInteractions = [...prev];
          newInteractions[index] = { ...newInteractions[index], id: result as number };
          return newInteractions;
        });
      } else {
        await updateInteractionMutation.mutateAsync({
          id: interaction.id,
          scenarioId: currentScenarioId,
          interactionType: interaction.interactionType,
          characterLabel: interaction.characterLabel,
          content: interaction.content,
          isSticky: interaction.isSticky,
          orderIndex: index,
        });
      }
      toast.success("Interaction saved");
    } catch (error) {
      toast.error("Failed to save interaction");
    }
  };

  const handleRemoveInteraction = async (index: number) => {
    const interaction = interactions[index];
    if (interaction.id && currentScenarioId) {
      try {
        await removeInteractionMutation.mutateAsync({ id: interaction.id, scenarioId: currentScenarioId });
      } catch (error) {
        toast.error("Failed to remove interaction");
        return;
      }
    }
    setInteractions(prev => prev.filter((_, i) => i !== index));
    toast.success("Interaction removed");
  };

  const getInteractionIcon = (type: string) => {
    switch (type) {
      case "message": return <MessageSquare className="h-4 w-4" />;
      case "text": return <BookOpen className="h-4 w-4" />;
      case "instruction": return <Wand2 className="h-4 w-4" />;
      default: return null;
    }
  };

  if (!isNew && isLoading) {
    return (
      <DashboardLayout>
        <div className="flex items-center justify-center h-96">
          <Loader2 className="h-8 w-8 animate-spin text-muted-foreground" />
        </div>
      </DashboardLayout>
    );
  }

  return (
    <DashboardLayout>
      <div className="space-y-6">
        {/* Header */}
        <div className="flex items-center justify-between">
          <div className="flex items-center gap-4">
            <Link href="/scenarios">
              <Button variant="ghost" size="sm">
                <ArrowLeft className="h-4 w-4 mr-2" />
                Back
              </Button>
            </Link>
            <h1 className="text-2xl font-bold">{isNew ? "New Scenario" : "Edit Scenario"}</h1>
          </div>
          <Button onClick={handleSave} disabled={isSaving}>
            {isSaving ? <Loader2 className="h-4 w-4 mr-2 animate-spin" /> : <Save className="h-4 w-4 mr-2" />}
            Save
          </Button>
        </div>

        <div className="grid lg:grid-cols-2 gap-6">
          {/* Left Column - Basic Info */}
          <div className="space-y-4">
            <Card>
              <CardHeader>
                <CardTitle className="text-lg">Basic Information</CardTitle>
              </CardHeader>
              <CardContent className="space-y-4">
                <div className="space-y-2">
                  <Label>Title</Label>
                  <Input
                    placeholder="Scenario title"
                    value={title}
                    onChange={(e) => setTitle(e.target.value)}
                  />
                </div>
                <div className="space-y-2">
                  <Label>Prompt Description (for AI)</Label>
                  <Textarea
                    placeholder="Describe the scenario setting, rules, and context for the AI..."
                    value={promptDescription}
                    onChange={(e) => setPromptDescription(e.target.value)}
                    className="min-h-[150px]"
                  />
                </div>
                <div className="space-y-2">
                  <Label>Display Description (for users)</Label>
                  <Textarea
                    placeholder="A brief description shown to users..."
                    value={displayDescription}
                    onChange={(e) => setDisplayDescription(e.target.value)}
                    className="min-h-[80px]"
                  />
                </div>
                <div className="flex items-center gap-2">
                  <Checkbox
                    id="isPublic"
                    checked={isPublic}
                    onCheckedChange={(checked) => setIsPublic(checked === true)}
                  />
                  <Label htmlFor="isPublic">Make this scenario public</Label>
                </div>
              </CardContent>
            </Card>

            <Card>
              <CardHeader className="flex flex-row items-center justify-between">
                <CardTitle className="text-lg">Characters</CardTitle>
                <Button size="sm" variant="ghost" onClick={handleAddCharacter}>
                  <Plus className="h-4 w-4" />
                </Button>
              </CardHeader>
              <CardContent className="space-y-4">
                {characters.length === 0 ? (
                  <p className="text-sm text-muted-foreground">No characters defined</p>
                ) : (
                  characters.map((char, index) => (
                    <div key={index} className="space-y-2 p-3 rounded-lg bg-secondary/50">
                      <div className="flex items-center gap-2">
                        <User className="h-4 w-4 text-muted-foreground" />
                        <Input
                          placeholder="Character name"
                          value={char.name}
                          onChange={(e) => {
                            const newChars = [...characters];
                            newChars[index] = { ...newChars[index], name: e.target.value };
                            setCharacters(newChars);
                          }}
                          className="flex-1"
                        />
                        <Button
                          size="sm"
                          variant="ghost"
                          onClick={() => handleRemoveCharacter(index)}
                        >
                          <Trash2 className="h-4 w-4 text-destructive" />
                        </Button>
                      </div>
                      <Input
                        placeholder="Label (e.g., alice, narrator)"
                        value={char.label}
                        onChange={(e) => {
                          const newChars = [...characters];
                          newChars[index] = { ...newChars[index], label: e.target.value.toLowerCase().replace(/[^a-z0-9_]/g, '') };
                          setCharacters(newChars);
                        }}
                      />
                      <Textarea
                        placeholder="Character description for AI..."
                        value={char.promptDescription}
                        onChange={(e) => {
                          const newChars = [...characters];
                          newChars[index] = { ...newChars[index], promptDescription: e.target.value };
                          setCharacters(newChars);
                        }}
                        className="min-h-[60px]"
                      />
                      <div className="flex items-center justify-between">
                        <div className="flex items-center gap-2">
                          <Checkbox
                            checked={char.isUserCharacter}
                            onCheckedChange={(checked) => {
                              const newChars = [...characters];
                              newChars[index] = { ...newChars[index], isUserCharacter: checked === true };
                              setCharacters(newChars);
                            }}
                          />
                          <Label className="text-sm">User character</Label>
                        </div>
                        <Button size="sm" variant="outline" onClick={() => handleSaveCharacter(index)}>
                          Save
                        </Button>
                      </div>
                    </div>
                  ))
                )}
              </CardContent>
            </Card>
          </div>

          {/* Right Column - Interactions */}
          <div>
            <Card>
              <CardHeader className="flex flex-row items-center justify-between">
                <CardTitle className="text-lg">Initial Interactions</CardTitle>
                <div className="flex gap-1">
                  <Button size="sm" variant="ghost" onClick={() => handleAddInteraction("message")} title="Add character message">
                    <MessageSquare className="h-4 w-4" />
                  </Button>
                  <Button size="sm" variant="ghost" onClick={() => handleAddInteraction("text")} title="Add narrator text">
                    <BookOpen className="h-4 w-4" />
                  </Button>
                  <Button size="sm" variant="ghost" onClick={() => handleAddInteraction("instruction")} title="Add instruction">
                    <Wand2 className="h-4 w-4" />
                  </Button>
                </div>
              </CardHeader>
              <CardContent className="space-y-4">
                {interactions.length === 0 ? (
                  <p className="text-sm text-muted-foreground">
                    No initial interactions. Add messages, narrator text, or instructions to set up the scenario.
                  </p>
                ) : (
                  interactions.map((interaction, index) => (
                    <div
                      key={index}
                      className={`space-y-2 p-3 rounded-lg ${
                        interaction.interactionType === "message" ? "message-character bg-card" :
                        interaction.interactionType === "text" ? "message-narrator bg-card" :
                        "message-instruction bg-card"
                      }`}
                    >
                      <div className="flex items-center justify-between">
                        <div className="flex items-center gap-2 text-sm text-muted-foreground">
                          {getInteractionIcon(interaction.interactionType)}
                          <span className="capitalize">{interaction.interactionType}</span>
                        </div>
                        <Button
                          size="sm"
                          variant="ghost"
                          onClick={() => handleRemoveInteraction(index)}
                        >
                          <Trash2 className="h-4 w-4 text-destructive" />
                        </Button>
                      </div>
                      
                      {interaction.interactionType === "message" && (
                        <Select
                          value={interaction.characterLabel || ""}
                          onValueChange={(value) => {
                            const newInteractions = [...interactions];
                            newInteractions[index] = { ...newInteractions[index], characterLabel: value };
                            setInteractions(newInteractions);
                          }}
                        >
                          <SelectTrigger>
                            <SelectValue placeholder="Select character" />
                          </SelectTrigger>
                          <SelectContent>
                            {characters.map((char) => (
                              <SelectItem key={char.label} value={char.label}>
                                {char.name} ({char.label})
                              </SelectItem>
                            ))}
                          </SelectContent>
                        </Select>
                      )}
                      
                      <Textarea
                        placeholder={
                          interaction.interactionType === "message" ? "Character's message..." :
                          interaction.interactionType === "text" ? "Narrator text..." :
                          "Instruction for AI..."
                        }
                        value={interaction.content}
                        onChange={(e) => {
                          const newInteractions = [...interactions];
                          newInteractions[index] = { ...newInteractions[index], content: e.target.value };
                          setInteractions(newInteractions);
                        }}
                        className="min-h-[80px]"
                      />
                      
                      <div className="flex items-center justify-between">
                        <div className="flex items-center gap-2">
                          <Checkbox
                            checked={interaction.isSticky}
                            onCheckedChange={(checked) => {
                              const newInteractions = [...interactions];
                              newInteractions[index] = { ...newInteractions[index], isSticky: checked === true };
                              setInteractions(newInteractions);
                            }}
                          />
                          <Label className="text-sm">Sticky (always visible)</Label>
                        </div>
                        <Button size="sm" variant="outline" onClick={() => handleSaveInteraction(index)}>
                          Save
                        </Button>
                      </div>
                    </div>
                  ))
                )}
              </CardContent>
            </Card>
          </div>
        </div>
      </div>
    </DashboardLayout>
  );
}
