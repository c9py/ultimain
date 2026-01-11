import DashboardLayout from "@/components/DashboardLayout";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { trpc } from "@/lib/trpc";
import { Copy, Loader2, Search, Wand2 } from "lucide-react";
import { useState } from "react";
import { useLocation } from "wouter";
import { toast } from "sonner";

export default function ScenarioLibrary() {
  const [, setLocation] = useLocation();
  const [searchQuery, setSearchQuery] = useState("");
  const { data: scenarios, isLoading, refetch } = trpc.scenarios.public.useQuery({ search: searchQuery || undefined });
  const copyMutation = trpc.scenarios.create.useMutation();

  const handleCopy = async (id: number, e: React.MouseEvent) => {
    e.stopPropagation();
    try {
      // Copy not implemented - just navigate to view
      toast.info("Scenario copying not yet implemented");
      setLocation(`/scenarios/${id}`);
    } catch (error) {
      toast.error("Failed to copy scenario");
    }
  };

  const handleSearch = (e: React.FormEvent) => {
    e.preventDefault();
    refetch();
  };

  return (
    <DashboardLayout>
      <div className="space-y-6">
        <div>
          <h1 className="text-3xl font-bold">Scenario Library</h1>
          <p className="text-muted-foreground mt-1">
            Browse and copy public scenarios created by the community
          </p>
        </div>

        <form onSubmit={handleSearch} className="flex gap-2 max-w-md">
          <div className="relative flex-1">
            <Search className="absolute left-3 top-1/2 -translate-y-1/2 h-4 w-4 text-muted-foreground" />
            <Input
              placeholder="Search scenarios..."
              value={searchQuery}
              onChange={(e) => setSearchQuery(e.target.value)}
              className="pl-10"
            />
          </div>
          <Button type="submit">Search</Button>
        </form>

        <div className="grid gap-4 md:grid-cols-2 lg:grid-cols-3">
          {isLoading ? (
            <div className="col-span-full flex items-center justify-center py-12">
              <Loader2 className="h-8 w-8 animate-spin text-muted-foreground" />
            </div>
          ) : scenarios && scenarios.length > 0 ? (
            scenarios.map((scenario: any) => (
              <Card
                key={scenario.id}
                className="hover:border-primary/50 transition-colors"
              >
                <CardHeader className="pb-2">
                  <CardTitle className="flex items-center gap-2 text-lg">
                    <Wand2 className="h-5 w-5 text-primary" />
                    {scenario.title}
                  </CardTitle>
                  <CardDescription>
                    {scenario.displayDescription || scenario.promptDescription?.slice(0, 100) || "No description"}
                  </CardDescription>
                </CardHeader>
                <CardContent>
                  <div className="flex items-center justify-between">
                    <span className="text-sm text-muted-foreground">
                      {new Date(scenario.createdAt).toLocaleDateString()}
                    </span>
                    <Button
                      size="sm"
                      variant="outline"
                      onClick={(e) => handleCopy(scenario.id, e)}
                      disabled={copyMutation.isPending}
                    >
                      {copyMutation.isPending ? (
                        <Loader2 className="h-4 w-4 animate-spin" />
                      ) : (
                        <>
                          <Copy className="h-4 w-4 mr-2" />
                          Copy
                        </>
                      )}
                    </Button>
                  </div>
                </CardContent>
              </Card>
            ))
          ) : (
            <Card className="col-span-full">
              <CardContent className="py-12 text-center">
                <Wand2 className="h-12 w-12 mx-auto text-muted-foreground mb-4" />
                <p className="text-muted-foreground">
                  {searchQuery ? "No scenarios found matching your search" : "No public scenarios available yet"}
                </p>
                <p className="text-sm text-muted-foreground mt-1">
                  Create your own scenarios and make them public to share with others
                </p>
              </CardContent>
            </Card>
          )}
        </div>
      </div>
    </DashboardLayout>
  );
}
