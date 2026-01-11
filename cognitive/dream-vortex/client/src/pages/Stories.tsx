import DashboardLayout from "@/components/DashboardLayout";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Dialog, DialogContent, DialogDescription, DialogFooter, DialogHeader, DialogTitle, DialogTrigger } from "@/components/ui/dialog";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { trpc } from "@/lib/trpc";
import { BookOpen, Loader2, Plus, Trash2 } from "lucide-react";
import { useState } from "react";
import { useLocation } from "wouter";
import { toast } from "sonner";

export default function Stories() {
  const [, setLocation] = useLocation();
  const { data: stories, isLoading, refetch } = trpc.stories.list.useQuery();
  const { data: apiKeys } = trpc.apiKeys.list.useQuery();
  const createMutation = trpc.stories.create.useMutation();
  const deleteMutation = trpc.stories.delete.useMutation();

  const [isDialogOpen, setIsDialogOpen] = useState(false);
  const [title, setTitle] = useState("");

  const hasApiKey = apiKeys && apiKeys.length > 0;

  const handleCreate = async () => {
    if (!title.trim()) {
      toast.error("Please enter a title");
      return;
    }

    try {
      const result = await createMutation.mutateAsync({ title: title.trim() });
      toast.success("Story created");
      setIsDialogOpen(false);
      setTitle("");
      if (result) setLocation(`/stories/${result}`);
    } catch (error) {
      toast.error("Failed to create story");
    }
  };

  const handleDelete = async (id: number, e: React.MouseEvent) => {
    e.stopPropagation();
    try {
      await deleteMutation.mutateAsync({ id });
      toast.success("Story deleted");
      refetch();
    } catch (error) {
      toast.error("Failed to delete story");
    }
  };

  return (
    <DashboardLayout>
      <div className="space-y-6">
        <div className="flex items-center justify-between">
          <div>
            <h1 className="text-3xl font-bold">Story Generator</h1>
            <p className="text-muted-foreground mt-1">
              Create and continue stories with AI assistance
            </p>
          </div>
          <Dialog open={isDialogOpen} onOpenChange={setIsDialogOpen}>
            <DialogTrigger asChild>
              <Button disabled={!hasApiKey}>
                <Plus className="h-4 w-4 mr-2" />
                New Story
              </Button>
            </DialogTrigger>
            <DialogContent>
              <DialogHeader>
                <DialogTitle>Create New Story</DialogTitle>
                <DialogDescription>
                  Start a new story. You can add plot details and characters later.
                </DialogDescription>
              </DialogHeader>
              <div className="space-y-4 py-4">
                <div className="space-y-2">
                  <Label htmlFor="title">Story Title</Label>
                  <Input
                    id="title"
                    placeholder="e.g., The Lost Kingdom"
                    value={title}
                    onChange={(e) => setTitle(e.target.value)}
                  />
                </div>
              </div>
              <DialogFooter>
                <Button variant="outline" onClick={() => setIsDialogOpen(false)}>
                  Cancel
                </Button>
                <Button onClick={handleCreate} disabled={createMutation.isPending}>
                  {createMutation.isPending && <Loader2 className="h-4 w-4 mr-2 animate-spin" />}
                  Create
                </Button>
              </DialogFooter>
            </DialogContent>
          </Dialog>
        </div>

        {!hasApiKey && (
          <Card className="border-yellow-500/50 bg-yellow-500/5">
            <CardContent className="pt-6">
              <p className="text-muted-foreground">
                Please configure your DreamGen API key first to generate stories.
              </p>
            </CardContent>
          </Card>
        )}

        <div className="grid gap-4">
          {isLoading ? (
            <div className="flex items-center justify-center py-12">
              <Loader2 className="h-8 w-8 animate-spin text-muted-foreground" />
            </div>
          ) : stories && stories.length > 0 ? (
            stories.map((story) => (
              <Card
                key={story.id}
                className="hover:border-primary/50 transition-colors cursor-pointer"
                onClick={() => setLocation(`/stories/${story.id}`)}
              >
                <CardHeader className="pb-2">
                  <div className="flex items-center justify-between">
                    <CardTitle className="flex items-center gap-2 text-lg">
                      <BookOpen className="h-5 w-5 text-primary" />
                      {story.title}
                    </CardTitle>
                    <Button
                      variant="ghost"
                      size="sm"
                      onClick={(e) => handleDelete(story.id, e)}
                    >
                      <Trash2 className="h-4 w-4 text-destructive" />
                    </Button>
                  </div>
                  <CardDescription>
                    {story.plotDescription ? story.plotDescription.slice(0, 100) + "..." : "No plot description"}
                    <br />
                    Updated {new Date(story.updatedAt).toLocaleDateString()}
                  </CardDescription>
                </CardHeader>
              </Card>
            ))
          ) : (
            <Card>
              <CardContent className="py-12 text-center">
                <BookOpen className="h-12 w-12 mx-auto text-muted-foreground mb-4" />
                <p className="text-muted-foreground">No stories yet</p>
                <p className="text-sm text-muted-foreground mt-1">
                  Create your first story to get started
                </p>
              </CardContent>
            </Card>
          )}
        </div>
      </div>
    </DashboardLayout>
  );
}
