import DashboardLayout from "@/components/DashboardLayout";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { trpc } from "@/lib/trpc";
import { Loader2, Plus, Trash2, Wand2 } from "lucide-react";
import { useLocation } from "wouter";
import { toast } from "sonner";

export default function Scenarios() {
  const [, setLocation] = useLocation();
  const { data: scenarios, isLoading, refetch } = trpc.scenarios.list.useQuery();
  const deleteMutation = trpc.scenarios.delete.useMutation();

  const handleDelete = async (id: number, e: React.MouseEvent) => {
    e.stopPropagation();
    try {
      await deleteMutation.mutateAsync({ id });
      toast.success("Scenario deleted");
      refetch();
    } catch (error) {
      toast.error("Failed to delete scenario");
    }
  };

  return (
    <DashboardLayout>
      <div className="space-y-6">
        <div className="flex items-center justify-between">
          <div>
            <h1 className="text-3xl font-bold">My Scenarios</h1>
            <p className="text-muted-foreground mt-1">
              Create and manage role-play scenarios with characters and interactions
            </p>
          </div>
          <Button onClick={() => setLocation("/scenarios/new")}>
            <Plus className="h-4 w-4 mr-2" />
            New Scenario
          </Button>
        </div>

        <div className="grid gap-4">
          {isLoading ? (
            <div className="flex items-center justify-center py-12">
              <Loader2 className="h-8 w-8 animate-spin text-muted-foreground" />
            </div>
          ) : scenarios && scenarios.length > 0 ? (
            scenarios.map((scenario) => (
              <Card
                key={scenario.id}
                className="hover:border-primary/50 transition-colors cursor-pointer"
                onClick={() => setLocation(`/scenarios/${scenario.id}`)}
              >
                <CardHeader className="pb-2">
                  <div className="flex items-center justify-between">
                    <CardTitle className="flex items-center gap-2 text-lg">
                      <Wand2 className="h-5 w-5 text-primary" />
                      {scenario.title}
                    </CardTitle>
                    <div className="flex items-center gap-2">
                      {scenario.isPublic && (
                        <span className="text-xs bg-primary/20 text-primary px-2 py-1 rounded">
                          Public
                        </span>
                      )}
                      <Button
                        variant="ghost"
                        size="sm"
                        onClick={(e) => handleDelete(scenario.id, e)}
                      >
                        <Trash2 className="h-4 w-4 text-destructive" />
                      </Button>
                    </div>
                  </div>
                  <CardDescription>
                    {scenario.displayDescription || scenario.promptDescription?.slice(0, 100) || "No description"}
                    <br />
                    Updated {new Date(scenario.updatedAt).toLocaleDateString()}
                  </CardDescription>
                </CardHeader>
              </Card>
            ))
          ) : (
            <Card>
              <CardContent className="py-12 text-center">
                <Wand2 className="h-12 w-12 mx-auto text-muted-foreground mb-4" />
                <p className="text-muted-foreground">No scenarios yet</p>
                <p className="text-sm text-muted-foreground mt-1">
                  Create your first scenario to define characters and settings
                </p>
              </CardContent>
            </Card>
          )}
        </div>
      </div>
    </DashboardLayout>
  );
}
