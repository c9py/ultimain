import DashboardLayout from "@/components/DashboardLayout";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select";
import { Slider } from "@/components/ui/slider";
import { Textarea } from "@/components/ui/textarea";
import { trpc } from "@/lib/trpc";
import { Image, Loader2, Sparkles, Trash2 } from "lucide-react";
import { useState } from "react";
import { toast } from "sonner";

const ASPECT_RATIOS = [
  { id: "square", name: "Square (1:1)" },
  { id: "portrait", name: "Portrait (3:4)" },
  { id: "landscape", name: "Landscape (4:3)" },
];

const STYLES = [
  { id: "none", name: "None" },
  { id: "anime", name: "Anime" },
  { id: "realistic", name: "Realistic" },
  { id: "fantasy", name: "Fantasy" },
  { id: "dark", name: "Dark/Gothic" },
  { id: "vibrant", name: "Vibrant" },
];

export default function ImageGenerator() {
  const { data: apiKeys } = trpc.apiKeys.list.useQuery();
  const { data: images, refetch: refetchImages } = trpc.images.list.useQuery({ limit: 20 });
  const saveMutation = trpc.images.create.useMutation();
  const deleteMutation = trpc.images.delete.useMutation();

  const [includePrompt, setIncludePrompt] = useState("");
  const [excludePrompt, setExcludePrompt] = useState("");
  const [cfgScale, setCfgScale] = useState(7);
  const [fidelity, setFidelity] = useState(30);
  const [aspectRatio, setAspectRatio] = useState("square");
  const [style, setStyle] = useState("none");
  const [seed, setSeed] = useState<number | undefined>(undefined);
  const [isGenerating, setIsGenerating] = useState(false);
  const [generatedImage, setGeneratedImage] = useState<string | null>(null);

  const hasApiKey = apiKeys && apiKeys.length > 0;

  const handleGenerate = async () => {
    if (!includePrompt.trim()) {
      toast.error("Please enter a prompt");
      return;
    }

    if (!hasApiKey) {
      toast.error("Please configure your API key first");
      return;
    }

    setIsGenerating(true);
    setGeneratedImage(null);

    try {
      // Simulate image generation - in production, this would call DreamGen's Muse API
      await new Promise(resolve => setTimeout(resolve, 2000));
      
      // Use a placeholder image for demo
      const placeholderUrl = `https://picsum.photos/seed/${Date.now()}/512/512`;
      setGeneratedImage(placeholderUrl);

      // Save to history
      await saveMutation.mutateAsync({
        includePrompt,
        excludePrompt: excludePrompt || undefined,
        cfgScale,
        fidelity,
        aspectRatio,
        style: style !== "none" ? style : undefined,
        seed,
        imageUrl: placeholderUrl,
      });

      refetchImages();
      toast.success("Image generated");
    } catch (error) {
      toast.error("Failed to generate image");
    } finally {
      setIsGenerating(false);
    }
  };

  const handleDelete = async (id: number) => {
    try {
      await deleteMutation.mutateAsync({ id });
      toast.success("Image deleted");
      refetchImages();
    } catch (error) {
      toast.error("Failed to delete image");
    }
  };

  return (
    <DashboardLayout>
      <div className="space-y-6">
        <div>
          <h1 className="text-3xl font-bold">Image Generator</h1>
          <p className="text-muted-foreground mt-1">
            Create images using the Muse model with natural or tag-based prompts
          </p>
        </div>

        {!hasApiKey && (
          <Card className="border-yellow-500/50 bg-yellow-500/5">
            <CardContent className="pt-6">
              <p className="text-muted-foreground">
                Please configure your DreamGen API key first to generate images.
              </p>
            </CardContent>
          </Card>
        )}

        <div className="grid lg:grid-cols-3 gap-6">
          {/* Left Column - Controls */}
          <div className="space-y-4">
            <Card>
              <CardHeader>
                <CardTitle className="text-lg">Prompt</CardTitle>
              </CardHeader>
              <CardContent className="space-y-4">
                <div className="space-y-2">
                  <Label>Include (what to generate)</Label>
                  <Textarea
                    placeholder="Describe what you want to see... (e.g., 'a mystical forest with glowing mushrooms')"
                    value={includePrompt}
                    onChange={(e) => setIncludePrompt(e.target.value)}
                    className="min-h-[100px]"
                  />
                </div>
                <div className="space-y-2">
                  <Label>Exclude (negative prompt)</Label>
                  <Textarea
                    placeholder="What to avoid... (e.g., 'blurry, low quality')"
                    value={excludePrompt}
                    onChange={(e) => setExcludePrompt(e.target.value)}
                    className="min-h-[60px]"
                  />
                </div>
              </CardContent>
            </Card>

            <Card>
              <CardHeader>
                <CardTitle className="text-lg">Settings</CardTitle>
              </CardHeader>
              <CardContent className="space-y-4">
                <div className="space-y-2">
                  <Label>Aspect Ratio</Label>
                  <Select value={aspectRatio} onValueChange={setAspectRatio}>
                    <SelectTrigger>
                      <SelectValue />
                    </SelectTrigger>
                    <SelectContent>
                      {ASPECT_RATIOS.map((ratio) => (
                        <SelectItem key={ratio.id} value={ratio.id}>
                          {ratio.name}
                        </SelectItem>
                      ))}
                    </SelectContent>
                  </Select>
                </div>

                <div className="space-y-2">
                  <Label>Style</Label>
                  <Select value={style} onValueChange={setStyle}>
                    <SelectTrigger>
                      <SelectValue />
                    </SelectTrigger>
                    <SelectContent>
                      {STYLES.map((s) => (
                        <SelectItem key={s.id} value={s.id}>
                          {s.name}
                        </SelectItem>
                      ))}
                    </SelectContent>
                  </Select>
                </div>

                <div className="space-y-2">
                  <Label>CFG Scale: {cfgScale}</Label>
                  <Slider
                    value={[cfgScale]}
                    onValueChange={([value]) => setCfgScale(value)}
                    min={1}
                    max={20}
                    step={0.5}
                  />
                  <p className="text-xs text-muted-foreground">
                    Higher values follow the prompt more closely
                  </p>
                </div>

                <div className="space-y-2">
                  <Label>Fidelity: {fidelity}</Label>
                  <Slider
                    value={[fidelity]}
                    onValueChange={([value]) => setFidelity(value)}
                    min={10}
                    max={50}
                    step={1}
                  />
                  <p className="text-xs text-muted-foreground">
                    Higher values produce more detailed images
                  </p>
                </div>

                <div className="space-y-2">
                  <Label>Seed (optional)</Label>
                  <Input
                    type="number"
                    placeholder="Random"
                    value={seed ?? ""}
                    onChange={(e) => setSeed(e.target.value ? parseInt(e.target.value) : undefined)}
                  />
                  <p className="text-xs text-muted-foreground">
                    Use the same seed for reproducible results
                  </p>
                </div>
              </CardContent>
            </Card>

            <Button
              className="w-full"
              size="lg"
              onClick={handleGenerate}
              disabled={isGenerating || !hasApiKey || !includePrompt.trim()}
            >
              {isGenerating ? (
                <>
                  <Loader2 className="h-4 w-4 mr-2 animate-spin" />
                  Generating...
                </>
              ) : (
                <>
                  <Sparkles className="h-4 w-4 mr-2" />
                  Generate Image
                </>
              )}
            </Button>
          </div>

          {/* Right Column - Result & History */}
          <div className="lg:col-span-2 space-y-4">
            {/* Current Result */}
            <Card>
              <CardHeader>
                <CardTitle className="text-lg">Generated Image</CardTitle>
              </CardHeader>
              <CardContent>
                {isGenerating ? (
                  <div className="aspect-square bg-secondary/50 rounded-lg flex items-center justify-center">
                    <div className="text-center">
                      <Loader2 className="h-12 w-12 animate-spin mx-auto text-primary mb-4" />
                      <p className="text-muted-foreground">Generating your image...</p>
                    </div>
                  </div>
                ) : generatedImage ? (
                  <img
                    src={generatedImage}
                    alt="Generated"
                    className="w-full rounded-lg"
                  />
                ) : (
                  <div className="aspect-square bg-secondary/50 rounded-lg flex items-center justify-center">
                    <div className="text-center">
                      <Image className="h-12 w-12 mx-auto text-muted-foreground mb-4" />
                      <p className="text-muted-foreground">Your generated image will appear here</p>
                    </div>
                  </div>
                )}
              </CardContent>
            </Card>

            {/* History */}
            <Card>
              <CardHeader>
                <CardTitle className="text-lg">Recent Generations</CardTitle>
              </CardHeader>
              <CardContent>
                {images && images.length > 0 ? (
                  <div className="grid grid-cols-3 md:grid-cols-4 gap-2">
                    {images.map((image) => (
                      <div key={image.id} className="relative group">
                        <img
                          src={image.imageUrl || ""}
                          alt={image.includePrompt}
                          className="aspect-square object-cover rounded-lg cursor-pointer hover:opacity-80 transition-opacity"
                          onClick={() => setGeneratedImage(image.imageUrl || null)}
                        />
                        <Button
                          size="sm"
                          variant="destructive"
                          className="absolute top-1 right-1 opacity-0 group-hover:opacity-100 transition-opacity h-6 w-6 p-0"
                          onClick={() => handleDelete(image.id)}
                        >
                          <Trash2 className="h-3 w-3" />
                        </Button>
                      </div>
                    ))}
                  </div>
                ) : (
                  <p className="text-sm text-muted-foreground text-center py-4">
                    No images generated yet
                  </p>
                )}
              </CardContent>
            </Card>
          </div>
        </div>
      </div>
    </DashboardLayout>
  );
}
