import DashboardLayout from "@/components/DashboardLayout";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Dialog, DialogContent, DialogDescription, DialogFooter, DialogHeader, DialogTitle, DialogTrigger } from "@/components/ui/dialog";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Textarea } from "@/components/ui/textarea";
import { Checkbox } from "@/components/ui/checkbox";
import { trpc } from "@/lib/trpc";
import { Edit, Loader2, Plus, Trash2, User } from "lucide-react";
import { useState } from "react";
import { toast } from "sonner";

interface CharacterForm {
  name: string;
  label: string;
  promptDescription: string;
  displayDescription: string;
  isUserCharacter: boolean;
}

const emptyForm: CharacterForm = {
  name: "",
  label: "",
  promptDescription: "",
  displayDescription: "",
  isUserCharacter: false,
};

export default function Characters() {
  const { data: characters, isLoading, refetch } = trpc.characters.list.useQuery();
  const createMutation = trpc.characters.create.useMutation();
  const updateMutation = trpc.characters.update.useMutation();
  const deleteMutation = trpc.characters.delete.useMutation();

  const [isDialogOpen, setIsDialogOpen] = useState(false);
  const [editingId, setEditingId] = useState<number | null>(null);
  const [form, setForm] = useState<CharacterForm>(emptyForm);

  const handleOpenCreate = () => {
    setEditingId(null);
    setForm(emptyForm);
    setIsDialogOpen(true);
  };

  const handleOpenEdit = (character: typeof characters extends (infer T)[] | undefined ? T : never) => {
    if (!character) return;
    setEditingId(character.id);
    setForm({
      name: character.name,
      label: character.label,
      promptDescription: character.promptDescription || "",
      displayDescription: character.displayDescription || "",
      isUserCharacter: character.isUserCharacter || false,
    });
    setIsDialogOpen(true);
  };

  const handleSave = async () => {
    if (!form.name.trim() || !form.label.trim()) {
      toast.error("Name and label are required");
      return;
    }

    if (!/^[a-z0-9_]+$/.test(form.label)) {
      toast.error("Label must be lowercase letters, numbers, and underscores only");
      return;
    }

    try {
      if (editingId) {
        await updateMutation.mutateAsync({
          id: editingId,
          ...form,
        });
        toast.success("Character updated");
      } else {
        await createMutation.mutateAsync(form);
        toast.success("Character created");
      }
      setIsDialogOpen(false);
      setForm(emptyForm);
      setEditingId(null);
      refetch();
    } catch (error) {
      toast.error("Failed to save character");
    }
  };

  const handleDelete = async (id: number) => {
    try {
      await deleteMutation.mutateAsync({ id });
      toast.success("Character deleted");
      refetch();
    } catch (error) {
      toast.error("Failed to delete character");
    }
  };

  return (
    <DashboardLayout>
      <div className="space-y-6">
        <div className="flex items-center justify-between">
          <div>
            <h1 className="text-3xl font-bold">Characters</h1>
            <p className="text-muted-foreground mt-1">
              Create and manage reusable characters for your scenarios and stories
            </p>
          </div>
          <Dialog open={isDialogOpen} onOpenChange={setIsDialogOpen}>
            <DialogTrigger asChild>
              <Button onClick={handleOpenCreate}>
                <Plus className="h-4 w-4 mr-2" />
                New Character
              </Button>
            </DialogTrigger>
            <DialogContent className="max-w-lg">
              <DialogHeader>
                <DialogTitle>{editingId ? "Edit Character" : "Create Character"}</DialogTitle>
                <DialogDescription>
                  Define a character that can be used across scenarios and stories.
                </DialogDescription>
              </DialogHeader>
              <div className="space-y-4 py-4">
                <div className="grid grid-cols-2 gap-4">
                  <div className="space-y-2">
                    <Label>Name</Label>
                    <Input
                      placeholder="e.g., Alice"
                      value={form.name}
                      onChange={(e) => setForm({ ...form, name: e.target.value })}
                    />
                  </div>
                  <div className="space-y-2">
                    <Label>Label</Label>
                    <Input
                      placeholder="e.g., alice"
                      value={form.label}
                      onChange={(e) => setForm({ ...form, label: e.target.value.toLowerCase().replace(/[^a-z0-9_]/g, '') })}
                    />
                    <p className="text-xs text-muted-foreground">Lowercase, no spaces</p>
                  </div>
                </div>
                <div className="space-y-2">
                  <Label>Prompt Description (for AI)</Label>
                  <Textarea
                    placeholder="Detailed description of the character for the AI..."
                    value={form.promptDescription}
                    onChange={(e) => setForm({ ...form, promptDescription: e.target.value })}
                    className="min-h-[100px]"
                  />
                </div>
                <div className="space-y-2">
                  <Label>Display Description (for users)</Label>
                  <Textarea
                    placeholder="Brief description shown to users..."
                    value={form.displayDescription}
                    onChange={(e) => setForm({ ...form, displayDescription: e.target.value })}
                    className="min-h-[60px]"
                  />
                </div>
                <div className="flex items-center gap-2">
                  <Checkbox
                    id="isUserCharacter"
                    checked={form.isUserCharacter}
                    onCheckedChange={(checked) => setForm({ ...form, isUserCharacter: checked === true })}
                  />
                  <Label htmlFor="isUserCharacter">This is a user/player character</Label>
                </div>
              </div>
              <DialogFooter>
                <Button variant="outline" onClick={() => setIsDialogOpen(false)}>
                  Cancel
                </Button>
                <Button onClick={handleSave} disabled={createMutation.isPending || updateMutation.isPending}>
                  {(createMutation.isPending || updateMutation.isPending) && (
                    <Loader2 className="h-4 w-4 mr-2 animate-spin" />
                  )}
                  {editingId ? "Update" : "Create"}
                </Button>
              </DialogFooter>
            </DialogContent>
          </Dialog>
        </div>

        <div className="grid gap-4 md:grid-cols-2 lg:grid-cols-3">
          {isLoading ? (
            <div className="col-span-full flex items-center justify-center py-12">
              <Loader2 className="h-8 w-8 animate-spin text-muted-foreground" />
            </div>
          ) : characters && characters.length > 0 ? (
            characters.map((character) => (
              <Card key={character.id} className="hover:border-primary/50 transition-colors">
                <CardHeader className="pb-2">
                  <div className="flex items-center justify-between">
                    <CardTitle className="flex items-center gap-2 text-lg">
                      <User className="h-5 w-5 text-primary" />
                      {character.name}
                    </CardTitle>
                    <div className="flex items-center gap-1">
                      <Button
                        variant="ghost"
                        size="sm"
                        onClick={() => handleOpenEdit(character)}
                      >
                        <Edit className="h-4 w-4" />
                      </Button>
                      <Button
                        variant="ghost"
                        size="sm"
                        onClick={() => handleDelete(character.id)}
                      >
                        <Trash2 className="h-4 w-4 text-destructive" />
                      </Button>
                    </div>
                  </div>
                  <CardDescription className="flex items-center gap-2">
                    <code className="text-xs bg-secondary px-1 rounded">{character.label}</code>
                    {character.isUserCharacter && (
                      <span className="text-xs bg-primary/20 text-primary px-2 py-0.5 rounded">
                        User
                      </span>
                    )}
                  </CardDescription>
                </CardHeader>
                <CardContent>
                  <p className="text-sm text-muted-foreground line-clamp-3">
                    {character.displayDescription || character.promptDescription || "No description"}
                  </p>
                </CardContent>
              </Card>
            ))
          ) : (
            <Card className="col-span-full">
              <CardContent className="py-12 text-center">
                <User className="h-12 w-12 mx-auto text-muted-foreground mb-4" />
                <p className="text-muted-foreground">No characters yet</p>
                <p className="text-sm text-muted-foreground mt-1">
                  Create reusable characters for your scenarios and stories
                </p>
              </CardContent>
            </Card>
          )}
        </div>
      </div>
    </DashboardLayout>
  );
}
