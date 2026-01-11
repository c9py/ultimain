import { useAuth } from "@/_core/hooks/useAuth";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Textarea } from "@/components/ui/textarea";
import { trpc } from "@/lib/trpc";
import { Building2, DollarSign, Loader2, Rocket } from "lucide-react";
import { useState } from "react";
import { useLocation } from "wouter";
import { toast } from "sonner";

export default function CompanySetup() {
  const { user, loading: authLoading } = useAuth();
  const [, setLocation] = useLocation();
  const [name, setName] = useState("");
  const [description, setDescription] = useState("");

  const { data: existingCompany, isLoading: companyLoading } =
    trpc.company.mine.useQuery();

  const createCompany = trpc.company.create.useMutation({
    onSuccess: () => {
      toast.success("Company created successfully!");
      setLocation("/dashboard");
    },
    onError: (error) => {
      toast.error(error.message);
    },
  });

  // Redirect if already has company
  if (!companyLoading && existingCompany) {
    setLocation("/dashboard");
    return null;
  }

  if (authLoading || companyLoading) {
    return (
      <div className="min-h-screen flex items-center justify-center bg-background">
        <Loader2 className="w-8 h-8 animate-spin text-primary" />
      </div>
    );
  }

  if (!user) {
    setLocation("/");
    return null;
  }

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    if (!name.trim()) {
      toast.error("Please enter a company name");
      return;
    }
    createCompany.mutate({ name: name.trim(), description: description.trim() || undefined });
  };

  return (
    <div className="min-h-screen bg-background flex items-center justify-center p-4">
      <div className="w-full max-w-lg">
        <div className="text-center mb-8">
          <div className="w-16 h-16 rounded-xl bg-primary flex items-center justify-center mx-auto mb-4">
            <Building2 className="w-8 h-8 text-primary-foreground" />
          </div>
          <h1 className="text-3xl font-bold mb-2">Welcome to Virtunomics</h1>
          <p className="text-muted-foreground">
            Let's set up your company and start building your empire
          </p>
        </div>

        <Card>
          <CardHeader>
            <CardTitle>Create Your Company</CardTitle>
            <CardDescription>
              Choose a name for your business. You'll start with $1,000,000 in
              capital to build your empire.
            </CardDescription>
          </CardHeader>
          <CardContent>
            <form onSubmit={handleSubmit} className="space-y-6">
              <div className="space-y-2">
                <Label htmlFor="name">Company Name</Label>
                <Input
                  id="name"
                  placeholder="Enter your company name"
                  value={name}
                  onChange={(e) => setName(e.target.value)}
                  maxLength={128}
                  required
                />
                <p className="text-xs text-muted-foreground">
                  This will be visible to other players
                </p>
              </div>

              <div className="space-y-2">
                <Label htmlFor="description">Description (Optional)</Label>
                <Textarea
                  id="description"
                  placeholder="Describe your company's mission..."
                  value={description}
                  onChange={(e) => setDescription(e.target.value)}
                  rows={3}
                />
              </div>

              <div className="bg-muted/50 rounded-lg p-4 space-y-3">
                <h4 className="font-medium flex items-center gap-2">
                  <Rocket className="w-4 h-4 text-primary" />
                  Starting Benefits
                </h4>
                <ul className="space-y-2 text-sm text-muted-foreground">
                  <li className="flex items-center gap-2">
                    <DollarSign className="w-4 h-4 text-success" />
                    $1,000,000 starting capital
                  </li>
                  <li className="flex items-center gap-2">
                    <Building2 className="w-4 h-4 text-primary" />
                    Access to all business unit types
                  </li>
                </ul>
              </div>

              <Button
                type="submit"
                className="w-full"
                size="lg"
                disabled={createCompany.isPending || !name.trim()}
              >
                {createCompany.isPending ? (
                  <>
                    <Loader2 className="w-4 h-4 mr-2 animate-spin" />
                    Creating Company...
                  </>
                ) : (
                  <>
                    <Rocket className="w-4 h-4 mr-2" />
                    Found Your Company
                  </>
                )}
              </Button>
            </form>
          </CardContent>
        </Card>

        <p className="text-center text-sm text-muted-foreground mt-6">
          By creating a company, you agree to compete fairly with other players
        </p>
      </div>
    </div>
  );
}
