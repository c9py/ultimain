import { useAuth } from "@/_core/hooks/useAuth";
import GameLayout from "@/components/GameLayout";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { trpc } from "@/lib/trpc";
import {
  Building2,
  DollarSign,
  Factory,
  TrendingUp,
  Users,
  Plus,
  ArrowUpRight,
  ArrowDownRight,
  Bell,
  Loader2,
  PlayCircle,
  FlaskConical,
  Clock,
} from "lucide-react";
import { toast } from "sonner";
import { useLocation } from "wouter";

export default function Dashboard() {
  const { user } = useAuth();
  const [, setLocation] = useLocation();

  const { data: company, isLoading: companyLoading, refetch: refetchCompany } =
    trpc.company.mine.useQuery();
  const { data: units, isLoading: unitsLoading } =
    trpc.businessUnit.list.useQuery();
  const { data: transactions, refetch: refetchTransactions } = trpc.transaction.list.useQuery({ limit: 5 });
  const { data: notifications } = trpc.notification.list.useQuery();
  const { data: gameState, refetch: refetchGameState } = trpc.game.state.useQuery();
  const { data: companyTech } = trpc.technology.companyTechnologies.useQuery();
  const { data: readyProduction } = trpc.gameTurn.readyProduction.useQuery();

  const advanceTurnMutation = trpc.gameTurn.advance.useMutation({
    onSuccess: (result) => {
      toast.success(`Turn ${result.turn} complete! Production: ${result.productionCompleted}, Research: ${result.researchAdvanced}`);
      refetchCompany();
      refetchTransactions();
      refetchGameState();
    },
    onError: (error) => {
      toast.error(error.message);
    },
  });

  // Redirect to setup if no company
  if (!companyLoading && !company) {
    setLocation("/setup");
    return null;
  }

  if (companyLoading || unitsLoading) {
    return (
      <GameLayout>
        <div className="flex items-center justify-center h-96">
          <Loader2 className="w-8 h-8 animate-spin text-primary" />
        </div>
      </GameLayout>
    );
  }

  const unreadNotifications =
    notifications?.filter((n) => !n.isRead).length || 0;

  // Calculate stats
  const totalUnits = units?.length || 0;
  const activeUnits = units?.filter((u) => u.isActive).length || 0;
  const cash = company ? parseFloat(company.cash) : 0;

  // Group units by type
  const unitsByType = units?.reduce(
    (acc, unit) => {
      acc[unit.type] = (acc[unit.type] || 0) + 1;
      return acc;
    },
    {} as Record<string, number>
  );

  return (
    <GameLayout>
      <div className="space-y-6">
        {/* Header */}
        <div className="flex flex-col md:flex-row md:items-center justify-between gap-4">
          <div>
            <h1 className="text-3xl font-bold">{company?.name}</h1>
            <p className="text-muted-foreground">
              Turn {gameState?.currentTurn || 1} • Founded{" "}
              {company?.founded
                ? new Date(company.founded).toLocaleDateString()
                : "Today"}
            </p>
          </div>
          <div className="flex items-center gap-3">
            <Button
              variant="outline"
              size="icon"
              className="relative"
              onClick={() => {}}
            >
              <Bell className="w-5 h-5" />
              {unreadNotifications > 0 && (
                <span className="absolute -top-1 -right-1 w-5 h-5 bg-destructive text-destructive-foreground text-xs rounded-full flex items-center justify-center">
                  {unreadNotifications}
                </span>
              )}
            </Button>
            <Button
              variant="default"
              onClick={() => advanceTurnMutation.mutate()}
              disabled={advanceTurnMutation.isPending}
              className="bg-amber-600 hover:bg-amber-700"
            >
              {advanceTurnMutation.isPending ? (
                <Loader2 className="w-4 h-4 mr-2 animate-spin" />
              ) : (
                <PlayCircle className="w-4 h-4 mr-2" />
              )}
              Advance Turn
            </Button>
            <Button onClick={() => setLocation("/units")}>
              <Plus className="w-4 h-4 mr-2" />
              New Unit
            </Button>
          </div>
        </div>

        {/* Stats Grid */}
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
          <Card>
            <CardContent className="pt-6">
              <div className="flex items-center justify-between">
                <div>
                  <p className="text-sm text-muted-foreground">Cash Balance</p>
                  <p className="text-2xl font-bold tabular-nums">
                    ${cash.toLocaleString()}
                  </p>
                </div>
                <div className="w-12 h-12 rounded-full bg-success/10 flex items-center justify-center">
                  <DollarSign className="w-6 h-6 text-success" />
                </div>
              </div>
            </CardContent>
          </Card>

          <Card>
            <CardContent className="pt-6">
              <div className="flex items-center justify-between">
                <div>
                  <p className="text-sm text-muted-foreground">Business Units</p>
                  <p className="text-2xl font-bold tabular-nums">
                    {activeUnits}{" "}
                    <span className="text-sm text-muted-foreground font-normal">
                      / {totalUnits}
                    </span>
                  </p>
                </div>
                <div className="w-12 h-12 rounded-full bg-primary/10 flex items-center justify-center">
                  <Building2 className="w-6 h-6 text-primary" />
                </div>
              </div>
            </CardContent>
          </Card>

          <Card>
            <CardContent className="pt-6">
              <div className="flex items-center justify-between">
                <div>
                  <p className="text-sm text-muted-foreground">Reputation</p>
                  <p className="text-2xl font-bold tabular-nums">
                    {company?.reputation || 50}
                    <span className="text-sm text-muted-foreground font-normal">
                      /100
                    </span>
                  </p>
                </div>
                <div className="w-12 h-12 rounded-full bg-warning/10 flex items-center justify-center">
                  <TrendingUp className="w-6 h-6 text-warning" />
                </div>
              </div>
            </CardContent>
          </Card>

          <Card>
            <CardContent className="pt-6">
              <div className="flex items-center justify-between">
                <div>
                  <p className="text-sm text-muted-foreground">Employees</p>
                  <p className="text-2xl font-bold tabular-nums">0</p>
                </div>
                <div className="w-12 h-12 rounded-full bg-blue-500/10 flex items-center justify-center">
                  <Users className="w-6 h-6 text-blue-500" />
                </div>
              </div>
            </CardContent>
          </Card>
        </div>

        {/* Main Content Grid */}
        <div className="grid lg:grid-cols-3 gap-6">
          {/* Business Units Overview */}
          <Card className="lg:col-span-2">
            <CardHeader className="flex flex-row items-center justify-between">
              <CardTitle>Business Units</CardTitle>
              <Button variant="ghost" size="sm" onClick={() => setLocation("/units")}>
                View All
              </Button>
            </CardHeader>
            <CardContent>
              {units && units.length > 0 ? (
                <div className="space-y-3">
                  {units.slice(0, 5).map((unit) => (
                    <div
                      key={unit.id}
                      className="flex items-center justify-between p-3 rounded-lg bg-muted/50 hover:bg-muted cursor-pointer transition-colors"
                      onClick={() => setLocation(`/units/${unit.id}`)}
                    >
                      <div className="flex items-center gap-3">
                        <div
                          className={`w-10 h-10 rounded-lg flex items-center justify-center ${
                            unit.type === "factory"
                              ? "bg-orange-500/20"
                              : unit.type === "store"
                                ? "bg-green-500/20"
                                : unit.type === "office"
                                  ? "bg-blue-500/20"
                                  : unit.type === "mine"
                                    ? "bg-stone-500/20"
                                    : unit.type === "farm"
                                      ? "bg-lime-500/20"
                                      : "bg-purple-500/20"
                          }`}
                        >
                          <Factory
                            className={`w-5 h-5 ${
                              unit.type === "factory"
                                ? "text-orange-500"
                                : unit.type === "store"
                                  ? "text-green-500"
                                  : unit.type === "office"
                                    ? "text-blue-500"
                                    : unit.type === "mine"
                                      ? "text-stone-500"
                                      : unit.type === "farm"
                                        ? "text-lime-500"
                                        : "text-purple-500"
                            }`}
                          />
                        </div>
                        <div>
                          <p className="font-medium">{unit.name}</p>
                          <p className="text-sm text-muted-foreground capitalize">
                            {unit.type} • Level {unit.level}
                          </p>
                        </div>
                      </div>
                      <div className="text-right">
                        <p className="text-sm">
                          {unit.isActive ? (
                            <span className="text-success">Active</span>
                          ) : (
                            <span className="text-muted-foreground">Inactive</span>
                          )}
                        </p>
                        <p className="text-xs text-muted-foreground">
                          {unit.condition}% condition
                        </p>
                      </div>
                    </div>
                  ))}
                </div>
              ) : (
                <div className="text-center py-8">
                  <Factory className="w-12 h-12 mx-auto text-muted-foreground mb-4" />
                  <p className="text-muted-foreground mb-4">
                    No business units yet
                  </p>
                  <Button onClick={() => setLocation("/units")}>
                    <Plus className="w-4 h-4 mr-2" />
                    Create Your First Unit
                  </Button>
                </div>
              )}
            </CardContent>
          </Card>

          {/* Recent Transactions */}
          <Card>
            <CardHeader className="flex flex-row items-center justify-between">
              <CardTitle>Recent Transactions</CardTitle>
              <Button variant="ghost" size="sm" onClick={() => setLocation("/finance")}>
                View All
              </Button>
            </CardHeader>
            <CardContent>
              {transactions && transactions.length > 0 ? (
                <div className="space-y-3">
                  {transactions.map((tx) => (
                    <div
                      key={tx.id}
                      className="flex items-center justify-between py-2 border-b border-border last:border-0"
                    >
                      <div className="flex items-center gap-3">
                        <div
                          className={`w-8 h-8 rounded-full flex items-center justify-center ${
                            parseFloat(tx.amount) >= 0
                              ? "bg-success/10"
                              : "bg-destructive/10"
                          }`}
                        >
                          {parseFloat(tx.amount) >= 0 ? (
                            <ArrowUpRight className="w-4 h-4 text-success" />
                          ) : (
                            <ArrowDownRight className="w-4 h-4 text-destructive" />
                          )}
                        </div>
                        <div>
                          <p className="text-sm font-medium capitalize">
                            {tx.type}
                          </p>
                          <p className="text-xs text-muted-foreground">
                            {new Date(tx.createdAt).toLocaleDateString()}
                          </p>
                        </div>
                      </div>
                      <p
                        className={`font-medium tabular-nums ${
                          parseFloat(tx.amount) >= 0
                            ? "text-success"
                            : "text-destructive"
                        }`}
                      >
                        {parseFloat(tx.amount) >= 0 ? "+" : ""}$
                        {Math.abs(parseFloat(tx.amount)).toLocaleString()}
                      </p>
                    </div>
                  ))}
                </div>
              ) : (
                <div className="text-center py-8">
                  <DollarSign className="w-12 h-12 mx-auto text-muted-foreground mb-4" />
                  <p className="text-muted-foreground">No transactions yet</p>
                </div>
              )}
            </CardContent>
          </Card>
        </div>

        {/* Unit Types Summary */}
        {unitsByType && Object.keys(unitsByType).length > 0 && (
          <Card>
            <CardHeader>
              <CardTitle>Units by Type</CardTitle>
            </CardHeader>
            <CardContent>
              <div className="grid grid-cols-2 md:grid-cols-3 lg:grid-cols-6 gap-4">
                {Object.entries(unitsByType).map(([type, count]) => (
                  <div
                    key={type}
                    className="text-center p-4 rounded-lg bg-muted/50"
                  >
                    <p className="text-2xl font-bold">{count}</p>
                    <p className="text-sm text-muted-foreground capitalize">
                      {type}s
                    </p>
                  </div>
                ))}
              </div>
            </CardContent>
          </Card>
        )}

        {/* Research & Production Status */}
        <div className="grid lg:grid-cols-2 gap-6">
          {/* Active Research */}
          <Card>
            <CardHeader className="flex flex-row items-center justify-between">
              <CardTitle className="flex items-center gap-2">
                <FlaskConical className="w-5 h-5" />
                Research Progress
              </CardTitle>
            </CardHeader>
            <CardContent>
              {companyTech && companyTech.length > 0 ? (
                <div className="space-y-3">
                  {companyTech.map((tech) => (
                    <div
                      key={tech.companyTech.id}
                      className="p-3 rounded-lg bg-muted/50"
                    >
                      <div className="flex items-center justify-between mb-2">
                        <span className="font-medium">{tech.technology?.name || "Unknown"}</span>
                        <span className={`text-sm ${
                          tech.companyTech.isCompleted
                            ? "text-success"
                            : tech.companyTech.researchProgress > 0
                              ? "text-amber-500"
                              : "text-muted-foreground"
                        }`}>
                          {tech.companyTech.isCompleted
                            ? "Complete"
                            : `${tech.companyTech.researchProgress}%`}
                        </span>
                      </div>
                      <div className="w-full bg-muted rounded-full h-2">
                        <div
                          className={`h-2 rounded-full ${
                            tech.companyTech.isCompleted
                              ? "bg-success"
                              : "bg-amber-500"
                          }`}
                          style={{ width: `${Math.min(100, tech.companyTech.researchProgress)}%` }}
                        />
                      </div>
                    </div>
                  ))}
                </div>
              ) : (
                <div className="text-center py-6">
                  <FlaskConical className="w-12 h-12 mx-auto text-muted-foreground mb-3" />
                  <p className="text-muted-foreground">No active research</p>
                  <p className="text-sm text-muted-foreground mt-1">
                    Start researching technologies to unlock new capabilities
                  </p>
                </div>
              )}
            </CardContent>
          </Card>

          {/* Production Ready */}
          <Card>
            <CardHeader className="flex flex-row items-center justify-between">
              <CardTitle className="flex items-center gap-2">
                <Clock className="w-5 h-5" />
                Ready for Collection
              </CardTitle>
            </CardHeader>
            <CardContent>
              {readyProduction && readyProduction.length > 0 ? (
                <div className="space-y-3">
                  {readyProduction.slice(0, 5).map((item) => (
                    <div
                      key={item.queue.id}
                      className="flex items-center justify-between p-3 rounded-lg bg-muted/50"
                    >
                      <div>
                        <p className="font-medium">{item.recipe?.description || "Production Item"}</p>
                        <p className="text-sm text-muted-foreground">
                          Quantity: {parseFloat(item.queue.quantity).toFixed(0)}
                        </p>
                      </div>
                      <span className="text-success text-sm font-medium">Ready!</span>
                    </div>
                  ))}
                  {readyProduction.length > 5 && (
                    <p className="text-sm text-muted-foreground text-center">
                      +{readyProduction.length - 5} more items ready
                    </p>
                  )}
                </div>
              ) : (
                <div className="text-center py-6">
                  <Clock className="w-12 h-12 mx-auto text-muted-foreground mb-3" />
                  <p className="text-muted-foreground">No production ready</p>
                  <p className="text-sm text-muted-foreground mt-1">
                    Production items will appear here when complete
                  </p>
                </div>
              )}
            </CardContent>
          </Card>
        </div>
      </div>
    </GameLayout>
  );
}
