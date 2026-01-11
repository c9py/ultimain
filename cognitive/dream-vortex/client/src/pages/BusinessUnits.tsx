import GameLayout from "@/components/GameLayout";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import {
  Dialog,
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogTrigger,
} from "@/components/ui/dialog";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/components/ui/select";
import { trpc } from "@/lib/trpc";
import {
  Building2,
  Factory,
  FlaskConical,
  Loader2,
  MapPin,
  Plus,
  ShoppingBag,
  Tractor,
  Pickaxe,
} from "lucide-react";
import { useState } from "react";
import { useLocation } from "wouter";
import { toast } from "sonner";

const unitTypes = [
  {
    type: "office",
    name: "Office",
    icon: Building2,
    description: "Central management, HR, and advertising",
    baseCost: 50000,
    color: "blue",
  },
  {
    type: "store",
    name: "Store",
    icon: ShoppingBag,
    description: "Retail sales to end consumers",
    baseCost: 100000,
    color: "green",
  },
  {
    type: "factory",
    name: "Factory",
    icon: Factory,
    description: "Manufacturing and production",
    baseCost: 500000,
    color: "orange",
  },
  {
    type: "mine",
    name: "Mine",
    icon: Pickaxe,
    description: "Resource extraction",
    baseCost: 1000000,
    color: "stone",
  },
  {
    type: "farm",
    name: "Farm",
    icon: Tractor,
    description: "Agricultural production",
    baseCost: 200000,
    color: "lime",
  },
  {
    type: "laboratory",
    name: "Laboratory",
    icon: FlaskConical,
    description: "Research and development",
    baseCost: 750000,
    color: "purple",
  },
] as const;

export default function BusinessUnits() {
  const [, setLocation] = useLocation();
  const [isCreateOpen, setIsCreateOpen] = useState(false);
  const [selectedType, setSelectedType] = useState<string>("");
  const [unitName, setUnitName] = useState("");
  const [selectedCity, setSelectedCity] = useState<string>("");
  const [unitSize, setUnitSize] = useState(100);

  const { data: company } = trpc.company.mine.useQuery();
  const { data: units, isLoading, refetch } = trpc.businessUnit.list.useQuery();
  const { data: cities } = trpc.city.list.useQuery();

  const createUnit = trpc.businessUnit.create.useMutation({
    onSuccess: () => {
      toast.success("Business unit created successfully!");
      setIsCreateOpen(false);
      setSelectedType("");
      setUnitName("");
      setSelectedCity("");
      setUnitSize(100);
      refetch();
    },
    onError: (error) => {
      toast.error(error.message);
    },
  });

  const handleCreate = () => {
    if (!selectedType || !unitName.trim() || !selectedCity) {
      toast.error("Please fill in all fields");
      return;
    }
    createUnit.mutate({
      type: selectedType as any,
      name: unitName.trim(),
      cityId: parseInt(selectedCity),
      size: unitSize,
    });
  };

  const selectedTypeInfo = unitTypes.find((t) => t.type === selectedType);
  const estimatedCost = selectedTypeInfo
    ? selectedTypeInfo.baseCost * (unitSize / 100)
    : 0;

  const getUnitIcon = (type: string) => {
    const unitType = unitTypes.find((t) => t.type === type);
    return unitType?.icon || Factory;
  };

  const getUnitColor = (type: string) => {
    const colors: Record<string, string> = {
      office: "bg-blue-500/20 text-blue-500",
      store: "bg-green-500/20 text-green-500",
      factory: "bg-orange-500/20 text-orange-500",
      mine: "bg-stone-500/20 text-stone-500",
      farm: "bg-lime-500/20 text-lime-500",
      laboratory: "bg-purple-500/20 text-purple-500",
    };
    return colors[type] || "bg-muted text-muted-foreground";
  };

  return (
    <GameLayout>
      <div className="space-y-6">
        {/* Header */}
        <div className="flex flex-col md:flex-row md:items-center justify-between gap-4">
          <div>
            <h1 className="text-3xl font-bold">Business Units</h1>
            <p className="text-muted-foreground">
              Manage your factories, stores, and other business operations
            </p>
          </div>
          <Dialog open={isCreateOpen} onOpenChange={setIsCreateOpen}>
            <DialogTrigger asChild>
              <Button>
                <Plus className="w-4 h-4 mr-2" />
                New Unit
              </Button>
            </DialogTrigger>
            <DialogContent className="max-w-2xl">
              <DialogHeader>
                <DialogTitle>Create Business Unit</DialogTitle>
                <DialogDescription>
                  Choose a type and location for your new business unit
                </DialogDescription>
              </DialogHeader>

              <div className="space-y-6 py-4">
                {/* Unit Type Selection */}
                <div className="space-y-3">
                  <Label>Unit Type</Label>
                  <div className="grid grid-cols-2 md:grid-cols-3 gap-3">
                    {unitTypes.map((type) => (
                      <button
                        key={type.type}
                        onClick={() => setSelectedType(type.type)}
                        className={`p-4 rounded-lg border text-left transition-all ${
                          selectedType === type.type
                            ? "border-primary bg-primary/10"
                            : "border-border hover:border-primary/50"
                        }`}
                      >
                        <type.icon
                          className={`w-6 h-6 mb-2 ${
                            selectedType === type.type
                              ? "text-primary"
                              : "text-muted-foreground"
                          }`}
                        />
                        <p className="font-medium text-sm">{type.name}</p>
                        <p className="text-xs text-muted-foreground mt-1">
                          ${type.baseCost.toLocaleString()}
                        </p>
                      </button>
                    ))}
                  </div>
                  {selectedTypeInfo && (
                    <p className="text-sm text-muted-foreground">
                      {selectedTypeInfo.description}
                    </p>
                  )}
                </div>

                {/* Unit Name */}
                <div className="space-y-2">
                  <Label htmlFor="unitName">Unit Name</Label>
                  <Input
                    id="unitName"
                    placeholder="Enter a name for this unit"
                    value={unitName}
                    onChange={(e) => setUnitName(e.target.value)}
                    maxLength={128}
                  />
                </div>

                {/* City Selection */}
                <div className="space-y-2">
                  <Label>Location</Label>
                  <Select value={selectedCity} onValueChange={setSelectedCity}>
                    <SelectTrigger>
                      <SelectValue placeholder="Select a city" />
                    </SelectTrigger>
                    <SelectContent>
                      {cities?.map((city) => (
                        <SelectItem key={city.id} value={city.id.toString()}>
                          <div className="flex items-center gap-2">
                            <MapPin className="w-4 h-4" />
                            {city.name}, {city.country}
                            <span className="text-muted-foreground text-xs">
                              (Tax: {(parseFloat(city.taxRate) * 100).toFixed(0)}%)
                            </span>
                          </div>
                        </SelectItem>
                      ))}
                    </SelectContent>
                  </Select>
                </div>

                {/* Size */}
                <div className="space-y-2">
                  <Label htmlFor="size">Size (m²)</Label>
                  <Input
                    id="size"
                    type="number"
                    min={50}
                    max={10000}
                    value={unitSize}
                    onChange={(e) => setUnitSize(parseInt(e.target.value) || 100)}
                  />
                  <p className="text-xs text-muted-foreground">
                    Larger units cost more but can accommodate more employees and
                    equipment
                  </p>
                </div>

                {/* Cost Summary */}
                {selectedType && (
                  <div className="bg-muted/50 rounded-lg p-4">
                    <div className="flex justify-between items-center">
                      <span className="text-sm text-muted-foreground">
                        Estimated Cost
                      </span>
                      <span className="text-lg font-bold">
                        ${estimatedCost.toLocaleString()}
                      </span>
                    </div>
                    {company && (
                      <div className="flex justify-between items-center mt-2">
                        <span className="text-sm text-muted-foreground">
                          Available Cash
                        </span>
                        <span
                          className={`text-sm font-medium ${
                            parseFloat(company.cash) >= estimatedCost
                              ? "text-success"
                              : "text-destructive"
                          }`}
                        >
                          ${parseFloat(company.cash).toLocaleString()}
                        </span>
                      </div>
                    )}
                  </div>
                )}
              </div>

              <DialogFooter>
                <Button
                  variant="outline"
                  onClick={() => setIsCreateOpen(false)}
                >
                  Cancel
                </Button>
                <Button
                  onClick={handleCreate}
                  disabled={
                    createUnit.isPending ||
                    !selectedType ||
                    !unitName.trim() ||
                    !selectedCity ||
                    !!(company && parseFloat(company.cash) < estimatedCost)
                  }
                >
                  {createUnit.isPending ? (
                    <>
                      <Loader2 className="w-4 h-4 mr-2 animate-spin" />
                      Creating...
                    </>
                  ) : (
                    <>
                      <Plus className="w-4 h-4 mr-2" />
                      Create Unit
                    </>
                  )}
                </Button>
              </DialogFooter>
            </DialogContent>
          </Dialog>
        </div>

        {/* Units Grid */}
        {isLoading ? (
          <div className="flex items-center justify-center h-64">
            <Loader2 className="w-8 h-8 animate-spin text-primary" />
          </div>
        ) : units && units.length > 0 ? (
          <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-4">
            {units.map((unit) => {
              const Icon = getUnitIcon(unit.type);
              return (
                <Card
                  key={unit.id}
                  className="cursor-pointer hover:border-primary/50 transition-colors"
                  onClick={() => setLocation(`/units/${unit.id}`)}
                >
                  <CardHeader className="pb-3">
                    <div className="flex items-start justify-between">
                      <div
                        className={`w-12 h-12 rounded-lg flex items-center justify-center ${getUnitColor(unit.type)}`}
                      >
                        <Icon className="w-6 h-6" />
                      </div>
                      <span
                        className={`text-xs px-2 py-1 rounded ${
                          unit.isActive
                            ? "bg-success/20 text-success"
                            : "bg-muted text-muted-foreground"
                        }`}
                      >
                        {unit.isActive ? "Active" : "Inactive"}
                      </span>
                    </div>
                    <CardTitle className="mt-3">{unit.name}</CardTitle>
                    <CardDescription className="capitalize">
                      {unit.type} • Level {unit.level}
                    </CardDescription>
                  </CardHeader>
                  <CardContent>
                    <div className="space-y-2">
                      <div className="flex justify-between text-sm">
                        <span className="text-muted-foreground">Size</span>
                        <span>{unit.size} m²</span>
                      </div>
                      <div className="flex justify-between text-sm">
                        <span className="text-muted-foreground">Condition</span>
                        <span>{unit.condition}%</span>
                      </div>
                      <div className="flex justify-between text-sm">
                        <span className="text-muted-foreground">Efficiency</span>
                        <span>{(parseFloat(unit.efficiency) * 100).toFixed(0)}%</span>
                      </div>
                    </div>
                    {/* Condition Bar */}
                    <div className="mt-4">
                      <div className="game-progress">
                        <div
                          className="game-progress-bar"
                          style={{ width: `${unit.condition}%` }}
                        />
                      </div>
                    </div>
                  </CardContent>
                </Card>
              );
            })}
          </div>
        ) : (
          <Card>
            <CardContent className="flex flex-col items-center justify-center py-16">
              <Factory className="w-16 h-16 text-muted-foreground mb-4" />
              <h3 className="text-xl font-semibold mb-2">No Business Units Yet</h3>
              <p className="text-muted-foreground text-center mb-6 max-w-md">
                Start building your empire by creating your first business unit.
                Choose from offices, stores, factories, and more.
              </p>
              <Button onClick={() => setIsCreateOpen(true)}>
                <Plus className="w-4 h-4 mr-2" />
                Create Your First Unit
              </Button>
            </CardContent>
          </Card>
        )}

        {/* Unit Types Info */}
        <Card>
          <CardHeader>
            <CardTitle>Unit Types</CardTitle>
            <CardDescription>
              Different types of business units serve different purposes
            </CardDescription>
          </CardHeader>
          <CardContent>
            <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-4">
              {unitTypes.map((type) => (
                <div
                  key={type.type}
                  className="flex items-start gap-3 p-3 rounded-lg bg-muted/50"
                >
                  <div className={`w-10 h-10 rounded-lg flex items-center justify-center ${getUnitColor(type.type)}`}>
                    <type.icon className="w-5 h-5" />
                  </div>
                  <div>
                    <p className="font-medium">{type.name}</p>
                    <p className="text-xs text-muted-foreground">
                      {type.description}
                    </p>
                    <p className="text-xs text-primary mt-1">
                      From ${type.baseCost.toLocaleString()}
                    </p>
                  </div>
                </div>
              ))}
            </div>
          </CardContent>
        </Card>
      </div>
    </GameLayout>
  );
}
