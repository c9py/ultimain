import GameLayout from "@/components/GameLayout";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Switch } from "@/components/ui/switch";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { trpc } from "@/lib/trpc";
import {
  ArrowLeft,
  Building2,
  Factory,
  FlaskConical,
  Loader2,
  Package,
  Pickaxe,
  Save,
  Settings,
  ShoppingBag,
  Tractor,
  Users,
} from "lucide-react";
import { useEffect, useState } from "react";
import { useLocation, useParams } from "wouter";
import { toast } from "sonner";

const unitIcons: Record<string, any> = {
  office: Building2,
  store: ShoppingBag,
  factory: Factory,
  mine: Pickaxe,
  farm: Tractor,
  laboratory: FlaskConical,
};

const unitColors: Record<string, string> = {
  office: "bg-blue-500/20 text-blue-500",
  store: "bg-green-500/20 text-green-500",
  factory: "bg-orange-500/20 text-orange-500",
  mine: "bg-stone-500/20 text-stone-500",
  farm: "bg-lime-500/20 text-lime-500",
  laboratory: "bg-purple-500/20 text-purple-500",
};

export default function UnitDetail() {
  const params = useParams<{ id: string }>();
  const [, setLocation] = useLocation();
  const unitId = parseInt(params.id || "0");

  const [editName, setEditName] = useState("");
  const [isActive, setIsActive] = useState(true);
  const [employeeCount, setEmployeeCount] = useState(0);
  const [salary, setSalary] = useState(1000);

  const { data: unit, isLoading, refetch } = trpc.businessUnit.byId.useQuery(
    { id: unitId },
    { enabled: unitId > 0 }
  );

  const { data: employees, refetch: refetchEmployees } =
    trpc.businessUnit.employees.useQuery(
      { unitId },
      { enabled: unitId > 0 }
    );

  const { data: inventory } = trpc.businessUnit.inventory.useQuery(
    { unitId },
    { enabled: unitId > 0 }
  );

  const { data: city } = trpc.city.byId.useQuery(
    { id: unit?.cityId || 0 },
    { enabled: !!unit?.cityId }
  );

  const updateUnit = trpc.businessUnit.update.useMutation({
    onSuccess: () => {
      toast.success("Unit updated successfully");
      refetch();
    },
    onError: (error) => {
      toast.error(error.message);
    },
  });

  const updateEmployees = trpc.businessUnit.updateEmployees.useMutation({
    onSuccess: () => {
      toast.success("Employees updated successfully");
      refetchEmployees();
    },
    onError: (error) => {
      toast.error(error.message);
    },
  });

  useEffect(() => {
    if (unit) {
      setEditName(unit.name);
      setIsActive(unit.isActive ?? true);
    }
  }, [unit]);

  useEffect(() => {
    if (employees) {
      setEmployeeCount(employees.count);
      setSalary(parseFloat(employees.salary));
    }
  }, [employees]);

  if (isLoading) {
    return (
      <GameLayout>
        <div className="flex items-center justify-center h-96">
          <Loader2 className="w-8 h-8 animate-spin text-primary" />
        </div>
      </GameLayout>
    );
  }

  if (!unit) {
    return (
      <GameLayout>
        <div className="flex flex-col items-center justify-center h-96">
          <Factory className="w-16 h-16 text-muted-foreground mb-4" />
          <h2 className="text-xl font-semibold mb-2">Unit Not Found</h2>
          <p className="text-muted-foreground mb-4">
            This business unit doesn't exist or you don't have access to it.
          </p>
          <Button onClick={() => setLocation("/units")}>
            <ArrowLeft className="w-4 h-4 mr-2" />
            Back to Units
          </Button>
        </div>
      </GameLayout>
    );
  }

  const Icon = unitIcons[unit.type] || Factory;
  const colorClass = unitColors[unit.type] || "bg-muted text-muted-foreground";

  const handleSaveSettings = () => {
    updateUnit.mutate({
      id: unitId,
      name: editName.trim() || undefined,
      isActive,
    });
  };

  const handleSaveEmployees = () => {
    updateEmployees.mutate({
      unitId,
      count: employeeCount,
      salary,
    });
  };

  return (
    <GameLayout>
      <div className="space-y-6">
        {/* Header */}
        <div className="flex items-center gap-4">
          <Button
            variant="ghost"
            size="icon"
            onClick={() => setLocation("/units")}
          >
            <ArrowLeft className="w-5 h-5" />
          </Button>
          <div className={`w-12 h-12 rounded-lg flex items-center justify-center ${colorClass}`}>
            <Icon className="w-6 h-6" />
          </div>
          <div>
            <h1 className="text-2xl font-bold">{unit.name}</h1>
            <p className="text-muted-foreground capitalize">
              {unit.type} • Level {unit.level}
              {city && ` • ${city.name}, ${city.country}`}
            </p>
          </div>
        </div>

        {/* Stats Overview */}
        <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
          <Card>
            <CardContent className="pt-6">
              <p className="text-sm text-muted-foreground">Status</p>
              <p className={`text-lg font-semibold ${unit.isActive ? "text-success" : "text-muted-foreground"}`}>
                {unit.isActive ? "Active" : "Inactive"}
              </p>
            </CardContent>
          </Card>
          <Card>
            <CardContent className="pt-6">
              <p className="text-sm text-muted-foreground">Size</p>
              <p className="text-lg font-semibold">{unit.size} m²</p>
            </CardContent>
          </Card>
          <Card>
            <CardContent className="pt-6">
              <p className="text-sm text-muted-foreground">Condition</p>
              <p className="text-lg font-semibold">{unit.condition}%</p>
            </CardContent>
          </Card>
          <Card>
            <CardContent className="pt-6">
              <p className="text-sm text-muted-foreground">Efficiency</p>
              <p className="text-lg font-semibold">
                {(parseFloat(unit.efficiency) * 100).toFixed(0)}%
              </p>
            </CardContent>
          </Card>
        </div>

        {/* Tabs */}
        <Tabs defaultValue="overview" className="space-y-4">
          <TabsList>
            <TabsTrigger value="overview">Overview</TabsTrigger>
            <TabsTrigger value="employees">
              <Users className="w-4 h-4 mr-2" />
              Employees
            </TabsTrigger>
            <TabsTrigger value="inventory">
              <Package className="w-4 h-4 mr-2" />
              Inventory
            </TabsTrigger>
            <TabsTrigger value="settings">
              <Settings className="w-4 h-4 mr-2" />
              Settings
            </TabsTrigger>
          </TabsList>

          <TabsContent value="overview">
            <div className="grid md:grid-cols-2 gap-6">
              <Card>
                <CardHeader>
                  <CardTitle>Unit Information</CardTitle>
                </CardHeader>
                <CardContent className="space-y-4">
                  <div className="flex justify-between">
                    <span className="text-muted-foreground">Type</span>
                    <span className="capitalize">{unit.type}</span>
                  </div>
                  <div className="flex justify-between">
                    <span className="text-muted-foreground">Level</span>
                    <span>{unit.level}</span>
                  </div>
                  <div className="flex justify-between">
                    <span className="text-muted-foreground">Size</span>
                    <span>{unit.size} m²</span>
                  </div>
                  <div className="flex justify-between">
                    <span className="text-muted-foreground">Created</span>
                    <span>{new Date(unit.createdAt).toLocaleDateString()}</span>
                  </div>
                </CardContent>
              </Card>

              <Card>
                <CardHeader>
                  <CardTitle>Location</CardTitle>
                </CardHeader>
                <CardContent className="space-y-4">
                  {city ? (
                    <>
                      <div className="flex justify-between">
                        <span className="text-muted-foreground">City</span>
                        <span>{city.name}</span>
                      </div>
                      <div className="flex justify-between">
                        <span className="text-muted-foreground">Country</span>
                        <span>{city.country}</span>
                      </div>
                      <div className="flex justify-between">
                        <span className="text-muted-foreground">Population</span>
                        <span>{city.population.toLocaleString()}</span>
                      </div>
                      <div className="flex justify-between">
                        <span className="text-muted-foreground">Tax Rate</span>
                        <span>{(parseFloat(city.taxRate) * 100).toFixed(0)}%</span>
                      </div>
                    </>
                  ) : (
                    <p className="text-muted-foreground">Loading location...</p>
                  )}
                </CardContent>
              </Card>
            </div>
          </TabsContent>

          <TabsContent value="employees">
            <Card>
              <CardHeader>
                <CardTitle>Employee Management</CardTitle>
                <CardDescription>
                  Manage the workforce at this unit
                </CardDescription>
              </CardHeader>
              <CardContent className="space-y-6">
                <div className="grid md:grid-cols-2 gap-6">
                  <div className="space-y-2">
                    <Label htmlFor="employeeCount">Number of Employees</Label>
                    <Input
                      id="employeeCount"
                      type="number"
                      min={0}
                      max={1000}
                      value={employeeCount}
                      onChange={(e) =>
                        setEmployeeCount(parseInt(e.target.value) || 0)
                      }
                    />
                    <p className="text-xs text-muted-foreground">
                      More employees increase production capacity
                    </p>
                  </div>
                  <div className="space-y-2">
                    <Label htmlFor="salary">Salary per Employee ($)</Label>
                    <Input
                      id="salary"
                      type="number"
                      min={100}
                      max={100000}
                      value={salary}
                      onChange={(e) => setSalary(parseInt(e.target.value) || 1000)}
                    />
                    <p className="text-xs text-muted-foreground">
                      Higher salaries improve morale and qualification
                    </p>
                  </div>
                </div>

                {employees && (
                  <div className="bg-muted/50 rounded-lg p-4 space-y-3">
                    <h4 className="font-medium">Current Status</h4>
                    <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
                      <div>
                        <p className="text-sm text-muted-foreground">Employees</p>
                        <p className="text-lg font-semibold">{employees.count}</p>
                      </div>
                      <div>
                        <p className="text-sm text-muted-foreground">Salary</p>
                        <p className="text-lg font-semibold">
                          ${parseFloat(employees.salary).toLocaleString()}
                        </p>
                      </div>
                      <div>
                        <p className="text-sm text-muted-foreground">Qualification</p>
                        <p className="text-lg font-semibold">
                          {(parseFloat(employees.qualification) * 100).toFixed(0)}%
                        </p>
                      </div>
                      <div>
                        <p className="text-sm text-muted-foreground">Morale</p>
                        <p className="text-lg font-semibold">{employees.morale}%</p>
                      </div>
                    </div>
                    <div className="pt-2">
                      <p className="text-sm text-muted-foreground">
                        Monthly Payroll: $
                        {(employees.count * parseFloat(employees.salary)).toLocaleString()}
                      </p>
                    </div>
                  </div>
                )}

                <Button
                  onClick={handleSaveEmployees}
                  disabled={updateEmployees.isPending}
                >
                  {updateEmployees.isPending ? (
                    <Loader2 className="w-4 h-4 mr-2 animate-spin" />
                  ) : (
                    <Save className="w-4 h-4 mr-2" />
                  )}
                  Save Changes
                </Button>
              </CardContent>
            </Card>
          </TabsContent>

          <TabsContent value="inventory">
            <Card>
              <CardHeader>
                <CardTitle>Inventory</CardTitle>
                <CardDescription>
                  Resources and goods stored at this unit
                </CardDescription>
              </CardHeader>
              <CardContent>
                {inventory && inventory.length > 0 ? (
                  <div className="space-y-3">
                    {inventory.map((item) => (
                      <div
                        key={item.inventory.id}
                        className="flex items-center justify-between p-3 rounded-lg bg-muted/50"
                      >
                        <div className="flex items-center gap-3">
                          <div className="w-10 h-10 rounded-lg bg-primary/10 flex items-center justify-center text-lg">
                            {item.resourceType?.icon || "📦"}
                          </div>
                          <div>
                            <p className="font-medium">
                              {item.resourceType?.name || "Unknown Resource"}
                            </p>
                            <p className="text-sm text-muted-foreground">
                              Quality: {(parseFloat(item.inventory.quality) * 100).toFixed(0)}%
                            </p>
                          </div>
                        </div>
                        <div className="text-right">
                          <p className="font-semibold">
                            {parseFloat(item.inventory.quantity).toLocaleString()}
                          </p>
                          <p className="text-sm text-muted-foreground">
                            {item.resourceType?.unit || "units"}
                          </p>
                        </div>
                      </div>
                    ))}
                  </div>
                ) : (
                  <div className="text-center py-8">
                    <Package className="w-12 h-12 mx-auto text-muted-foreground mb-4" />
                    <p className="text-muted-foreground">No inventory yet</p>
                    <p className="text-sm text-muted-foreground mt-1">
                      Purchase resources from the market or produce them
                    </p>
                  </div>
                )}
              </CardContent>
            </Card>
          </TabsContent>

          <TabsContent value="settings">
            <Card>
              <CardHeader>
                <CardTitle>Unit Settings</CardTitle>
                <CardDescription>
                  Configure this business unit
                </CardDescription>
              </CardHeader>
              <CardContent className="space-y-6">
                <div className="space-y-2">
                  <Label htmlFor="editName">Unit Name</Label>
                  <Input
                    id="editName"
                    value={editName}
                    onChange={(e) => setEditName(e.target.value)}
                    maxLength={128}
                  />
                </div>

                <div className="flex items-center justify-between">
                  <div>
                    <Label>Active Status</Label>
                    <p className="text-sm text-muted-foreground">
                      Inactive units don't produce or incur operating costs
                    </p>
                  </div>
                  <Switch
                    checked={isActive}
                    onCheckedChange={setIsActive}
                  />
                </div>

                <Button
                  onClick={handleSaveSettings}
                  disabled={updateUnit.isPending}
                >
                  {updateUnit.isPending ? (
                    <Loader2 className="w-4 h-4 mr-2 animate-spin" />
                  ) : (
                    <Save className="w-4 h-4 mr-2" />
                  )}
                  Save Settings
                </Button>
              </CardContent>
            </Card>
          </TabsContent>
        </Tabs>
      </div>
    </GameLayout>
  );
}
