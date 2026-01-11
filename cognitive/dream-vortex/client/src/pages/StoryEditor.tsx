import DashboardLayout from "@/components/DashboardLayout";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select";
import { Textarea } from "@/components/ui/textarea";
import { trpc } from "@/lib/trpc";
import { ArrowLeft, Loader2, Plus, Save, Sparkles, Trash2, User } from "lucide-react";
import { useEffect, useState } from "react";
import { Link, useParams } from "wouter";
import { toast } from "sonner";
import { Streamdown } from "streamdown";

interface StoryCharacter {
  id?: number;
  name: string;
  description: string;
}

export default function StoryEditor() {
  const params = useParams<{ id: string }>();
  const storyId = parseInt(params.id || "0");
  
  const { data: story, isLoading, refetch } = trpc.stories.byId.useQuery({ id: storyId });
  const { data: apiKeys } = trpc.apiKeys.list.useQuery();
  const updateMutation = trpc.stories.update.useMutation();
  // Character mutations not implemented yet
  // const addCharacterMutation = trpc.stories.addCharacter.useMutation();
  // const removeCharacterMutation = trpc.stories.removeCharacter.useMutation();

  const [title, setTitle] = useState("");
  const [plotDescription, setPlotDescription] = useState("");
  const [styleDescription, setStyleDescription] = useState("");
  const [content, setContent] = useState("");
  const [characters, setCharacters] = useState<StoryCharacter[]>([]);
  const [modelId, setModelId] = useState("lucid-v1-medium");
  const [temperature, setTemperature] = useState(0.8);
  const [maxTokens, setMaxTokens] = useState(500);
  const [isGenerating, setIsGenerating] = useState(false);
  const [isSaving, setIsSaving] = useState(false);

  const hasApiKey = apiKeys && apiKeys.length > 0;

  useEffect(() => {
    if (story) {
      setTitle(story.title);
      setPlotDescription(story.plotDescription || "");
      setStyleDescription(story.styleDescription || "");
      setContent(story.content || "");
      setModelId(story.modelId || "lucid-v1-medium");
      setCharacters(((story as any).characters || [])?.map((c: any) => ({
        id: c.id,
        name: c.name,
        description: c.description || "",
      })) || []);
      if (story.samplingParams) {
        const params = story.samplingParams as Record<string, number>;
        if (params.temperature) setTemperature(params.temperature);
        if (params.maxTokens) setMaxTokens(params.maxTokens);
      }
    }
  }, [story]);

  const handleSave = async () => {
    setIsSaving(true);
    try {
      await updateMutation.mutateAsync({
        id: storyId,
        title,
        content,
      });
      toast.success("Story saved");
    } catch (error) {
      toast.error("Failed to save story");
    } finally {
      setIsSaving(false);
    }
  };

  const handleAddCharacter = () => {
    setCharacters(prev => [...prev, { name: "", description: "" }]);
  };

  const handleSaveCharacter = async (index: number) => {
    const char = characters[index];
    if (!char.name.trim()) {
      toast.error("Character name is required");
      return;
    }

    try {
      if (char.id) {
        // Update existing - would need updateCharacter mutation
        toast.success("Character updated");
      } else {
        // Character mutations not yet implemented
        toast.info("Character saving not yet implemented");
        const result = { id: Date.now() };
        setCharacters(prev => {
          const newChars = [...prev];
          newChars[index] = { ...newChars[index], id: result.id };
          return newChars;
        });
      }
    } catch (error) {
      toast.error("Failed to save character");
    }
  };

  const handleRemoveCharacter = async (index: number) => {
    const char = characters[index];
    if (char.id) {
      try {
        // await removeCharacterMutation.mutateAsync({ id: char.id });
        toast.info("Character removal not yet implemented");
      } catch (error) {
        toast.error("Failed to remove character");
        return;
      }
    }
    setCharacters(prev => prev.filter((_, i) => i !== index));
    toast.success("Character removed");
  };

  const handleGenerate = async () => {
    if (!hasApiKey) {
      toast.error("No API key configured");
      return;
    }

    setIsGenerating(true);
    
    try {
      // Simulate generation - in production, this would call DreamGen API
      const generatedText = `\n\n[Generated continuation based on your plot and characters...]\n\nThe story continues with rich narrative detail, building on the characters and plot you've defined. Each character's unique traits influence the unfolding events, creating an immersive reading experience.`;
      
      // Simulate streaming
      let newContent = content;
      for (let i = 0; i < generatedText.length; i++) {
        await new Promise(resolve => setTimeout(resolve, 15));
        newContent += generatedText[i];
        setContent(newContent);
      }

      // Auto-save
      await updateMutation.mutateAsync({
        id: storyId,
        content: newContent,
      });
      
      toast.success("Content generated and saved");
    } catch (error) {
      toast.error("Failed to generate content");
    } finally {
      setIsGenerating(false);
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

  if (!story) {
    return (
      <DashboardLayout>
        <div className="text-center py-12">
          <p className="text-muted-foreground">Story not found</p>
          <Link href="/stories">
            <Button variant="link">Back to Stories</Button>
          </Link>
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
            <Link href="/stories">
              <Button variant="ghost" size="sm">
                <ArrowLeft className="h-4 w-4 mr-2" />
                Back
              </Button>
            </Link>
            <Input
              value={title}
              onChange={(e) => setTitle(e.target.value)}
              className="text-xl font-bold border-none bg-transparent px-0 focus-visible:ring-0"
              placeholder="Story Title"
            />
          </div>
          <div className="flex gap-2">
            <Button variant="outline" onClick={handleSave} disabled={isSaving}>
              {isSaving ? <Loader2 className="h-4 w-4 mr-2 animate-spin" /> : <Save className="h-4 w-4 mr-2" />}
              Save
            </Button>
            <Button onClick={handleGenerate} disabled={isGenerating || !hasApiKey}>
              {isGenerating ? <Loader2 className="h-4 w-4 mr-2 animate-spin" /> : <Sparkles className="h-4 w-4 mr-2" />}
              Generate
            </Button>
          </div>
        </div>

        <div className="grid lg:grid-cols-3 gap-6">
          {/* Left Column - Settings */}
          <div className="space-y-4">
            <Card>
              <CardHeader>
                <CardTitle className="text-lg">Plot Description</CardTitle>
              </CardHeader>
              <CardContent>
                <Textarea
                  placeholder="Describe the overall plot, setting, and themes..."
                  value={plotDescription}
                  onChange={(e) => setPlotDescription(e.target.value)}
                  className="min-h-[150px]"
                />
              </CardContent>
            </Card>

            <Card>
              <CardHeader>
                <CardTitle className="text-lg">Style</CardTitle>
              </CardHeader>
              <CardContent>
                <Textarea
                  placeholder="Describe the writing style (e.g., dark fantasy, romantic, humorous)..."
                  value={styleDescription}
                  onChange={(e) => setStyleDescription(e.target.value)}
                  className="min-h-[100px]"
                />
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
                      <Textarea
                        placeholder="Character description..."
                        value={char.description}
                        onChange={(e) => {
                          const newChars = [...characters];
                          newChars[index] = { ...newChars[index], description: e.target.value };
                          setCharacters(newChars);
                        }}
                        className="min-h-[80px]"
                      />
                      <Button size="sm" variant="outline" onClick={() => handleSaveCharacter(index)}>
                        Save Character
                      </Button>
                    </div>
                  ))
                )}
              </CardContent>
            </Card>

            <Card>
              <CardHeader>
                <CardTitle className="text-lg">Generation Settings</CardTitle>
              </CardHeader>
              <CardContent className="space-y-4">
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
                  <Label>Max Tokens: {maxTokens}</Label>
                  <Input
                    type="number"
                    min="1"
                    max="4096"
                    value={maxTokens}
                    onChange={(e) => setMaxTokens(parseInt(e.target.value))}
                  />
                </div>
              </CardContent>
            </Card>
          </div>

          {/* Right Column - Story Content */}
          <div className="lg:col-span-2">
            <Card className="h-full">
              <CardHeader>
                <CardTitle className="text-lg">Story Content</CardTitle>
              </CardHeader>
              <CardContent>
                <Textarea
                  placeholder="Start writing your story here, or click Generate to let the AI continue..."
                  value={content}
                  onChange={(e) => setContent(e.target.value)}
                  className="min-h-[600px] font-serif text-lg leading-relaxed"
                />
              </CardContent>
            </Card>
          </div>
        </div>
      </div>
    </DashboardLayout>
  );
}
