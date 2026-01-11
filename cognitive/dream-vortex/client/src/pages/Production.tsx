import { useState } from "react";
import { trpc } from "@/lib/trpc";
import GameLayout from "@/components/GameLayout";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Button } from "@/components/ui/button";
import { Badge } from "@/components/ui/badge";
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { Dialog, DialogContent, DialogDescription, DialogFooter, DialogHeader, DialogTitle, DialogTrigger } from "@/components/ui/dialog";
import { Loader2, Factory, Pickaxe, Wheat, FlaskConical, Play, Clock, Package, ArrowRight, Users } from "lucide-react";
import { useToast } from "@/hooks/use-toast";

export default function Production() {
  const { toast } = useToast();
  const [selectedUnit, setSelectedUnit] = useState<number | null>(null);
  const [selectedRecipe, setSelectedRecipe] = useState<number | null>(null);
  const [quantity, setQuantity] = useState(1);
  const [dialogOpen, setDialogOpen] = useState(false);

  const { data: units, isLoading: unitsLoading } = trpc.businessUnit.list.useQuery();
  const { data: resources } = trpc.resource.list.useQuery();
  
  // Filter units that can produce
  const productionUnits = units?.filter((u) => 
    ["factory", "farm", "mine", "laboratory"].includes(u.type)
  );

  // Get the selected unit
  const currentUnit = productionUnits?.find((u) => u.id === selectedUnit);

  // Get recipes for the selected unit type
  const { data: recipes } = trpc.production.recipes.useQuery(
    { unitType: currentUnit?.type as "factory" | "farm" | "mine" | "laboratory" },
    { enabled: !!currentUnit }
  );

  // Get production queue for selected unit
  const { data: productionQueue, refetch: refetchQueue } = trpc.production.queue.useQuery(
    { unitId: selectedUnit! },
    { enabled: !!selectedUnit }
  );

  // Get inventory for selected unit
  const { data: inventory } = trpc.businessUnit.inventory.useQuery(
    { unitId: selectedUnit! },
    { enabled: !!selectedUnit }
  );

  const startProduction = trpc.production.start.useMutation({
    onSuccess: () => {
      toast({
        title: "Production Started",
        description: "Your production has been queued successfully.",
      });
      setDialogOpen(false);
      setSelectedRecipe(null);
      setQuantity(1);
      refetchQueue();
    },
    onError: (error) => {
      toast({
        title: "Error",
        description: error.message,
        variant: "destructive",
      });
    },
  });

  const getUnitIcon = (type: string) => {
    switch (type) {
      case "factory":
        return <Factory className="w-5 h-5" />;
      case "mine":
        return <Pickaxe className="w-5 h-5" />;
      case "farm":
        return <Wheat className="w-5 h-5" />;
      case "laboratory":
        return <FlaskConical className="w-5 h-5" />;
      default:
        return <Package className="w-5 h-5" />;
    }
  };

  const getResourceName = (id: number) => {
    return resources?.find((r) => r.id === id)?.name || "Unknown";
  };

  const selectedRecipeData = recipes?.find((r) => r.id === selectedRecipe);

  const handleStartProduction = () => {
    if (!selectedUnit || !selectedRecipe) return;
    
    startProduction.mutate({
      unitId: selectedUnit,
      recipeId: selectedRecipe,
      quantity,
    });
  };

  if (unitsLoading) {
    return (
      <GameLayout>
        <div className="flex items-center justify-center h-64">
          <Loader2 className="w-8 h-8 animate-spin text-primary" />
        </div>
      </GameLayout>
    );
  }

  if (!productionUnits || productionUnits.length === 0) {
    return (
      <GameLayout>
        <div className="space-y-6">
          <div>
            <h1 className="text-3xl font-bold">Production</h1>
            <p className="text-muted-foreground">Manage production at your facilities</p>
          </div>
          
          <Card className="border-dashed">
            <CardContent className="flex flex-col items-center justify-center py-12">
              <Factory className="w-12 h-12 text-muted-foreground mb-4" />
              <h3 className="text-lg font-semibold mb-2">No Production Facilities</h3>
              <p className="text-muted-foreground text-center mb-4">
                You need to build a factory, mine, farm, or laboratory to start producing goods.
              </p>
              <Button asChild>
                <a href="/units">Build a Facility</a>
              </Button>
            </CardContent>
          </Card>
        </div>
      </GameLayout>
    );
  }

  return (
    <GameLayout>
      <div className="space-y-6">
        <div>
          <h1 className="text-3xl font-bold">Production</h1>
          <p className="text-muted-foreground">Manage production at your facilities</p>
        </div>

        <div className="grid gap-6 lg:grid-cols-3">
          {/* Unit Selection */}
          <Card>
            <CardHeader>
              <CardTitle>Select Facility</CardTitle>
              <CardDescription>Choose a production unit</CardDescription>
            </CardHeader>
            <CardContent className="space-y-2">
              {productionUnits.map((unit) => (
                <button
                  key={unit.id}
                  onClick={() => setSelectedUnit(unit.id)}
                  className={`w-full p-3 rounded-lg border text-left transition-colors ${
                    selectedUnit === unit.id
                      ? "border-primary bg-primary/10"
                      : "border-border hover:border-primary/50"
                  }`}
                >
                  <div className="flex items-center gap-3">
                    {getUnitIcon(unit.type)}
                    <div className="flex-1 min-w-0">
                      <p className="font-medium truncate">{unit.name}</p>
                      <p className="text-sm text-muted-foreground capitalize">{unit.type}</p>
                    </div>
                    <Badge variant={unit.isActive ? "default" : "secondary"}>
                      {unit.isActive ? "Active" : "Inactive"}
                    </Badge>
                  </div>
                </button>
              ))}
            </CardContent>
          </Card>

          {/* Production Panel */}
          <div className="lg:col-span-2 space-y-6">
            {selectedUnit && currentUnit ? (
              <Tabs defaultValue="recipes">
                <TabsList className="grid w-full grid-cols-3">
                  <TabsTrigger value="recipes">Recipes</TabsTrigger>
                  <TabsTrigger value="queue">Queue</TabsTrigger>
                  <TabsTrigger value="inventory">Inventory</TabsTrigger>
                </TabsList>

                <TabsContent value="recipes" className="space-y-4">
                  <Card>
                    <CardHeader>
                      <CardTitle>Available Recipes</CardTitle>
                      <CardDescription>
                        Production recipes available for {currentUnit.name}
                      </CardDescription>
                    </CardHeader>
                    <CardContent>
                      {recipes && recipes.length > 0 ? (
                        <div className="grid gap-4">
                          {recipes.map((recipe) => {
                            const inputs = (recipe.inputResources as { resourceId: number; quantity: number }[] | null) || [];
                            return (
                              <div
                                key={recipe.id}
                                className="p-4 rounded-lg border bg-card hover:border-primary/50 transition-colors"
                              >
                                <div className="flex items-start justify-between mb-3">
                                  <div>
                                    <h4 className="font-semibold">{recipe.description || "Production"}</h4>
                                    <div className="flex items-center gap-4 text-sm text-muted-foreground mt-1">
                                      <span className="flex items-center gap-1">
                                        <Clock className="w-4 h-4" />
                                        {recipe.timeRequired} turn(s)
                                      </span>
                                      <span className="flex items-center gap-1">
                                        <Users className="w-4 h-4" />
                                        {recipe.laborRequired} workers
                                      </span>
                                    </div>
                                  </div>
                                  <Dialog open={dialogOpen && selectedRecipe === recipe.id} onOpenChange={(open) => {
                                    setDialogOpen(open);
                                    if (open) setSelectedRecipe(recipe.id);
                                  }}>
                                    <DialogTrigger asChild>
                                      <Button size="sm">
                                        <Play className="w-4 h-4 mr-2" />
                                        Produce
                                      </Button>
                                    </DialogTrigger>
                                    <DialogContent>
                                      <DialogHeader>
                                        <DialogTitle>Start Production</DialogTitle>
                                        <DialogDescription>
                                          Configure production for {recipe.description}
                                        </DialogDescription>
                                      </DialogHeader>
                                      <div className="space-y-4 py-4">
                                        <div className="space-y-2">
                                          <Label>Quantity to Produce</Label>
                                          <Input
                                            type="number"
                                            min={1}
                                            value={quantity}
                                            onChange={(e) => setQuantity(parseInt(e.target.value) || 1)}
                                          />
                                        </div>
                                        <div className="p-4 rounded-lg bg-muted">
                                          <p className="text-sm font-medium mb-2">Production Summary</p>
                                          <div className="space-y-1 text-sm">
                                            <div className="flex justify-between">
                                              <span>Output:</span>
                                              <span className="font-medium text-green-500">
                                                +{(parseFloat(recipe.outputQuantity) * quantity).toFixed(0)} {getResourceName(recipe.outputResourceId)}
                                              </span>
                                            </div>
                                            <div className="flex justify-between">
                                              <span>Time:</span>
                                              <span>{recipe.timeRequired * quantity} turn(s)</span>
                                            </div>
                                          </div>
                                        </div>
                                      </div>
                                      <DialogFooter>
                                        <Button variant="outline" onClick={() => setDialogOpen(false)}>
                                          Cancel
                                        </Button>
                                        <Button onClick={handleStartProduction} disabled={startProduction.isPending}>
                                          {startProduction.isPending ? (
                                            <>
                                              <Loader2 className="w-4 h-4 mr-2 animate-spin" />
                                              Starting...
                                            </>
                                          ) : (
                                            "Start Production"
                                          )}
                                        </Button>
                                      </DialogFooter>
                                    </DialogContent>
                                  </Dialog>
                                </div>

                                {/* Recipe flow */}
                                <div className="flex items-center gap-2 flex-wrap">
                                  {inputs.length > 0 ? (
                                    <>
                                      <div className="flex items-center gap-1 flex-wrap">
                                        {inputs.map((input, idx) => (
                                          <Badge key={idx} variant="outline" className="text-orange-500">
                                            {input.quantity} {getResourceName(input.resourceId)}
                                          </Badge>
                                        ))}
                                      </div>
                                      <ArrowRight className="w-4 h-4 text-muted-foreground" />
                                    </>
                                  ) : (
                                    <>
                                      <Badge variant="outline" className="text-muted-foreground">
                                        No inputs required
                                      </Badge>
                                      <ArrowRight className="w-4 h-4 text-muted-foreground" />
                                    </>
                                  )}
                                  <Badge className="bg-green-500/20 text-green-500 border-green-500/50">
                                    {parseFloat(recipe.outputQuantity).toFixed(0)} {getResourceName(recipe.outputResourceId)}
                                  </Badge>
                                </div>
                              </div>
                            );
                          })}
                        </div>
                      ) : (
                        <p className="text-muted-foreground text-center py-8">
                          No recipes available for this unit type.
                        </p>
                      )}
                    </CardContent>
                  </Card>
                </TabsContent>

                <TabsContent value="queue">
                  <Card>
                    <CardHeader>
                      <CardTitle>Production Queue</CardTitle>
                      <CardDescription>
                        Current production jobs at {currentUnit.name}
                      </CardDescription>
                    </CardHeader>
                    <CardContent>
                      {productionQueue && productionQueue.length > 0 ? (
                        <div className="space-y-3">
                          {productionQueue.map((item) => (
                            <div
                              key={item.queue.id}
                              className="p-4 rounded-lg border bg-card"
                            >
                              <div className="flex items-center justify-between">
                                <div>
                                  <p className="font-medium">
                                    {item.recipe?.description || "Production"}
                                  </p>
                                  <p className="text-sm text-muted-foreground">
                                    Quantity: {parseFloat(item.queue.quantity).toFixed(0)}
                                  </p>
                                </div>
                                <div className="text-right">
                                  <Badge variant={item.queue.isActive ? "default" : "secondary"}>
                                    {item.queue.isActive ? "Active" : "Paused"}
                                  </Badge>
                                  <p className="text-sm text-muted-foreground mt-1">
                                    Progress: {parseFloat(item.queue.progress).toFixed(0)}%
                                  </p>
                                </div>
                              </div>
                            </div>
                          ))}
                        </div>
                      ) : (
                        <p className="text-muted-foreground text-center py-8">
                          No active production. Select a recipe to start producing.
                        </p>
                      )}
                    </CardContent>
                  </Card>
                </TabsContent>

                <TabsContent value="inventory">
                  <Card>
                    <CardHeader>
                      <CardTitle>Unit Inventory</CardTitle>
                      <CardDescription>
                        Resources stored at {currentUnit.name}
                      </CardDescription>
                    </CardHeader>
                    <CardContent>
                      {inventory && inventory.length > 0 ? (
                        <div className="grid gap-3 sm:grid-cols-2">
                          {inventory.map((item) => (
                            <div
                              key={item.inventory.id}
                              className="p-4 rounded-lg border bg-card"
                            >
                              <div className="flex items-center justify-between">
                                <div>
                                  <p className="font-medium">
                                    {item.resourceType?.name || "Unknown"}
                                  </p>
                                  <p className="text-sm text-muted-foreground">
                                    Quality: {(parseFloat(item.inventory.quality) * 100).toFixed(0)}%
                                  </p>
                                </div>
                                <div className="text-right">
                                  <p className="text-2xl font-bold">
                                    {parseFloat(item.inventory.quantity).toFixed(0)}
                                  </p>
                                  <p className="text-sm text-muted-foreground">
                                    {item.resourceType?.unit || "units"}
                                  </p>
                                </div>
                              </div>
                            </div>
                          ))}
                        </div>
                      ) : (
                        <p className="text-muted-foreground text-center py-8">
                          No inventory at this unit.
                        </p>
                      )}
                    </CardContent>
                  </Card>
                </TabsContent>
              </Tabs>
            ) : (
              <Card>
                <CardContent className="flex flex-col items-center justify-center py-12">
                  <Factory className="w-12 h-12 text-muted-foreground mb-4" />
                  <h3 className="text-lg font-semibold mb-2">Select a Facility</h3>
                  <p className="text-muted-foreground text-center">
                    Choose a production facility from the list to view recipes and manage production.
                  </p>
                </CardContent>
              </Card>
            )}
          </div>
        </div>
      </div>
    </GameLayout>
  );
}
