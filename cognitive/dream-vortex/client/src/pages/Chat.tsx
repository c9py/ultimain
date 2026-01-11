import DashboardLayout from "@/components/DashboardLayout";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Dialog, DialogContent, DialogDescription, DialogFooter, DialogHeader, DialogTitle, DialogTrigger } from "@/components/ui/dialog";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select";
import { trpc } from "@/lib/trpc";
import { Loader2, MessageSquare, Plus, Trash2 } from "lucide-react";
import { useState } from "react";
import { useLocation } from "wouter";
import { toast } from "sonner";

export default function Chat() {
  const [, setLocation] = useLocation();
  const { data: sessions, isLoading, refetch } = trpc.chat.sessions.useQuery();
  const { data: scenarios } = trpc.scenarios.list.useQuery();
  const { data: apiKeys } = trpc.apiKeys.list.useQuery();
  const createMutation = trpc.chat.createSession.useMutation();
  const deleteMutation = trpc.chat.deleteSession.useMutation();

  const [isDialogOpen, setIsDialogOpen] = useState(false);
  const [title, setTitle] = useState("");
  const [scenarioId, setScenarioId] = useState<string>("");
  const [modelId, setModelId] = useState("lucid-v1-medium");

  const hasApiKey = apiKeys && apiKeys.length > 0;

  const handleCreate = async () => {
    if (!title.trim()) {
      toast.error("Please enter a title");
      return;
    }

    try {
      const result = await createMutation.mutateAsync({
        title: title.trim(),
        scenarioId: scenarioId ? parseInt(scenarioId) : undefined,
        modelId,
      });
      toast.success("Chat session created");
      setIsDialogOpen(false);
      setTitle("");
      setScenarioId("");
      if (result) setLocation(`/chat/${result}`);
    } catch (error) {
      toast.error("Failed to create chat session");
    }
  };

  const handleDelete = async (id: number, e: React.MouseEvent) => {
    e.stopPropagation();
    try {
      await deleteMutation.mutateAsync({ id });
      toast.success("Chat session deleted");
      refetch();
    } catch (error) {
      toast.error("Failed to delete chat session");
    }
  };

  return (
    <DashboardLayout>
      <div className="space-y-6">
        <div className="flex items-center justify-between">
          <div>
            <h1 className="text-3xl font-bold">Role-Play Chat</h1>
            <p className="text-muted-foreground mt-1">
              Engage in immersive conversations with AI characters
            </p>
          </div>
          <Dialog open={isDialogOpen} onOpenChange={setIsDialogOpen}>
            <DialogTrigger asChild>
              <Button disabled={!hasApiKey}>
                <Plus className="h-4 w-4 mr-2" />
                New Chat
              </Button>
            </DialogTrigger>
            <DialogContent>
              <DialogHeader>
                <DialogTitle>Create New Chat Session</DialogTitle>
                <DialogDescription>
                  Start a new role-play conversation. You can optionally base it on a scenario.
                </DialogDescription>
              </DialogHeader>
              <div className="space-y-4 py-4">
                <div className="space-y-2">
                  <Label htmlFor="title">Session Title</Label>
                  <Input
                    id="title"
                    placeholder="e.g., Adventure in the Forest"
                    value={title}
                    onChange={(e) => setTitle(e.target.value)}
                  />
                </div>
                <div className="space-y-2">
                  <Label htmlFor="scenario">Based on Scenario (Optional)</Label>
                  <Select value={scenarioId} onValueChange={setScenarioId}>
                    <SelectTrigger>
                      <SelectValue placeholder="Select a scenario" />
                    </SelectTrigger>
                    <SelectContent>
                      <SelectItem value="">None</SelectItem>
                      {scenarios?.map((scenario) => (
                        <SelectItem key={scenario.id} value={scenario.id.toString()}>
                          {scenario.title}
                        </SelectItem>
                      ))}
                    </SelectContent>
                  </Select>
                </div>
                <div className="space-y-2">
                  <Label htmlFor="model">Model</Label>
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
                Please configure your DreamGen API key first to start chatting.
              </p>
            </CardContent>
          </Card>
        )}

        <div className="grid gap-4">
          {isLoading ? (
            <div className="flex items-center justify-center py-12">
              <Loader2 className="h-8 w-8 animate-spin text-muted-foreground" />
            </div>
          ) : sessions && sessions.length > 0 ? (
            sessions.map((session: any) => (
              <Card
                key={session.id}
                className="hover:border-primary/50 transition-colors cursor-pointer"
                onClick={() => setLocation(`/chat/${session.id}`)}
              >
                <CardHeader className="pb-2">
                  <div className="flex items-center justify-between">
                    <CardTitle className="flex items-center gap-2 text-lg">
                      <MessageSquare className="h-5 w-5 text-primary" />
                      {session.title}
                    </CardTitle>
                    <Button
                      variant="ghost"
                      size="sm"
                      onClick={(e) => handleDelete(session.id, e)}
                    >
                      <Trash2 className="h-4 w-4 text-destructive" />
                    </Button>
                  </div>
                  <CardDescription>
                    Model: {session.modelId} • Updated {new Date(session.updatedAt).toLocaleDateString()}
                  </CardDescription>
                </CardHeader>
              </Card>
            ))
          ) : (
            <Card>
              <CardContent className="py-12 text-center">
                <MessageSquare className="h-12 w-12 mx-auto text-muted-foreground mb-4" />
                <p className="text-muted-foreground">No chat sessions yet</p>
                <p className="text-sm text-muted-foreground mt-1">
                  Create your first chat to start role-playing
                </p>
              </CardContent>
            </Card>
          )}
        </div>
      </div>
    </DashboardLayout>
  );
}
