import { useAuth } from "@/_core/hooks/useAuth";
import DashboardLayout from "@/components/DashboardLayout";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Dialog, DialogContent, DialogDescription, DialogFooter, DialogHeader, DialogTitle, DialogTrigger } from "@/components/ui/dialog";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { trpc } from "@/lib/trpc";
import { Check, Key, Loader2, Plus, Trash2, X } from "lucide-react";
import { useState } from "react";
import { toast } from "sonner";

export default function ApiKeys() {
  const { data: apiKeys, isLoading, refetch } = trpc.apiKeys.list.useQuery();
  const createMutation = trpc.apiKeys.create.useMutation();
  const deleteMutation = trpc.apiKeys.delete.useMutation();
  const verifyMutation = trpc.apiKeys.verify.useMutation();

  const [isDialogOpen, setIsDialogOpen] = useState(false);
  const [keyName, setKeyName] = useState("");
  const [apiKey, setApiKey] = useState("");
  const [verifyingId, setVerifyingId] = useState<number | null>(null);
  const [verifyResults, setVerifyResults] = useState<Record<number, boolean>>({});

  const handleCreate = async () => {
    if (!keyName.trim() || !apiKey.trim()) {
      toast.error("Please fill in all fields");
      return;
    }

    try {
      await createMutation.mutateAsync({ keyName: keyName.trim(), encryptedKey: apiKey.trim() });
      toast.success("API key added successfully");
      setKeyName("");
      setApiKey("");
      setIsDialogOpen(false);
      refetch();
    } catch (error) {
      toast.error("Failed to add API key");
    }
  };

  const handleDelete = async (id: number) => {
    try {
      await deleteMutation.mutateAsync({ id });
      toast.success("API key deleted");
      refetch();
    } catch (error) {
      toast.error("Failed to delete API key");
    }
  };

  const handleVerify = async (id: number) => {
    setVerifyingId(id);
    try {
      const result = await verifyMutation.mutateAsync({ id });
      setVerifyResults(prev => ({ ...prev, [id]: result.valid }));
      if (result.valid) {
        toast.success("API key is valid");
        refetch();
      } else {
        toast.error(result.error || "API key is invalid");
      }
    } catch (error) {
      toast.error("Failed to verify API key");
      setVerifyResults(prev => ({ ...prev, [id]: false }));
    } finally {
      setVerifyingId(null);
    }
  };

  return (
    <DashboardLayout>
      <div className="space-y-6">
        <div className="flex items-center justify-between">
          <div>
            <h1 className="text-3xl font-bold">API Keys</h1>
            <p className="text-muted-foreground mt-1">
              Manage your DreamGen API keys for text and image generation
            </p>
          </div>
          <Dialog open={isDialogOpen} onOpenChange={setIsDialogOpen}>
            <DialogTrigger asChild>
              <Button>
                <Plus className="h-4 w-4 mr-2" />
                Add API Key
              </Button>
            </DialogTrigger>
            <DialogContent>
              <DialogHeader>
                <DialogTitle>Add DreamGen API Key</DialogTitle>
                <DialogDescription>
                  Enter your DreamGen API key. You can create one in your DreamGen account settings.
                </DialogDescription>
              </DialogHeader>
              <div className="space-y-4 py-4">
                <div className="space-y-2">
                  <Label htmlFor="keyName">Key Name</Label>
                  <Input
                    id="keyName"
                    placeholder="e.g., My Primary Key"
                    value={keyName}
                    onChange={(e) => setKeyName(e.target.value)}
                  />
                </div>
                <div className="space-y-2">
                  <Label htmlFor="apiKey">API Key</Label>
                  <Input
                    id="apiKey"
                    type="password"
                    placeholder="Enter your DreamGen API key"
                    value={apiKey}
                    onChange={(e) => setApiKey(e.target.value)}
                  />
                </div>
              </div>
              <DialogFooter>
                <Button variant="outline" onClick={() => setIsDialogOpen(false)}>
                  Cancel
                </Button>
                <Button onClick={handleCreate} disabled={createMutation.isPending}>
                  {createMutation.isPending && <Loader2 className="h-4 w-4 mr-2 animate-spin" />}
                  Add Key
                </Button>
              </DialogFooter>
            </DialogContent>
          </Dialog>
        </div>

        <Card>
          <CardHeader>
            <CardTitle className="flex items-center gap-2">
              <Key className="h-5 w-5" />
              Your API Keys
            </CardTitle>
            <CardDescription>
              Your API keys are encrypted and stored securely. Never share your keys with anyone.
            </CardDescription>
          </CardHeader>
          <CardContent>
            {isLoading ? (
              <div className="flex items-center justify-center py-8">
                <Loader2 className="h-6 w-6 animate-spin text-muted-foreground" />
              </div>
            ) : apiKeys && apiKeys.length > 0 ? (
              <div className="space-y-3">
                {apiKeys.map((key) => (
                  <div
                    key={key.id}
                    className="flex items-center justify-between p-4 rounded-lg border bg-card"
                  >
                    <div className="flex items-center gap-4">
                      <Key className="h-5 w-5 text-muted-foreground" />
                      <div>
                        <p className="font-medium">{key.keyName}</p>
                        <p className="text-sm text-muted-foreground">
                          Added {new Date(key.createdAt).toLocaleDateString()}
                          {key.lastUsed && ` • Last used ${new Date(key.lastUsed).toLocaleDateString()}`}
                        </p>
                      </div>
                    </div>
                    <div className="flex items-center gap-2">
                      {verifyResults[key.id] !== undefined && (
                        <span className={`flex items-center gap-1 text-sm ${verifyResults[key.id] ? 'text-green-500' : 'text-red-500'}`}>
                          {verifyResults[key.id] ? <Check className="h-4 w-4" /> : <X className="h-4 w-4" />}
                          {verifyResults[key.id] ? 'Valid' : 'Invalid'}
                        </span>
                      )}
                      <Button
                        variant="outline"
                        size="sm"
                        onClick={() => handleVerify(key.id)}
                        disabled={verifyingId === key.id}
                      >
                        {verifyingId === key.id ? (
                          <Loader2 className="h-4 w-4 animate-spin" />
                        ) : (
                          "Verify"
                        )}
                      </Button>
                      <Button
                        variant="ghost"
                        size="sm"
                        onClick={() => handleDelete(key.id)}
                        disabled={deleteMutation.isPending}
                      >
                        <Trash2 className="h-4 w-4 text-destructive" />
                      </Button>
                    </div>
                  </div>
                ))}
              </div>
            ) : (
              <div className="text-center py-8">
                <Key className="h-12 w-12 mx-auto text-muted-foreground mb-4" />
                <p className="text-muted-foreground">No API keys configured yet</p>
                <p className="text-sm text-muted-foreground mt-1">
                  Add your DreamGen API key to start generating content
                </p>
              </div>
            )}
          </CardContent>
        </Card>

        <Card>
          <CardHeader>
            <CardTitle>How to get your API key</CardTitle>
          </CardHeader>
          <CardContent className="space-y-4">
            <ol className="list-decimal list-inside space-y-2 text-muted-foreground">
              <li>Go to <a href="https://dreamgen.com" target="_blank" rel="noopener noreferrer" className="text-primary hover:underline">dreamgen.com</a> and sign in</li>
              <li>Navigate to your account settings</li>
              <li>Find the API section and create a new API key</li>
              <li>Copy the key and paste it here</li>
            </ol>
            <p className="text-sm text-muted-foreground">
              Note: DreamGen API access requires a premium subscription plan.
            </p>
          </CardContent>
        </Card>
      </div>
    </DashboardLayout>
  );
}
