import { useState } from "react";
import GameLayout from "../components/GameLayout";
import { trpc } from "../lib/trpc";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { Button } from "../components/ui/button";
import { Input } from "../components/ui/input";
import { Label } from "../components/ui/label";
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "../components/ui/select";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "../components/ui/tabs";
import { Badge } from "../components/ui/badge";
import { Dialog, DialogContent, DialogDescription, DialogHeader, DialogTitle, DialogTrigger } from "../components/ui/dialog";
import { Globe, Zap, TrendingUp, TrendingDown, AlertTriangle, Sparkles, BookOpen, Plus, Calendar, Activity } from "lucide-react";

export default function WorldEvents() {
  const [selectedWorldId, setSelectedWorldId] = useState<number | null>(null);
  const [createWorldDialogOpen, setCreateWorldDialogOpen] = useState(false);
  const [newWorldName, setNewWorldName] = useState("");
  const [newWorldGenre, setNewWorldGenre] = useState<string>("modern");
  const [newWorldDescription, setNewWorldDescription] = useState("");

  const { data: worlds, refetch: refetchWorlds } = trpc.world.list.useQuery();
  const { data: worldState } = trpc.world.byId.useQuery(
    { id: selectedWorldId! },
    { enabled: !!selectedWorldId }
  );
  // World events and lore queries
  const { data: worldEvents = [] } = trpc.worldEvent.byWorld.useQuery(
    { worldId: selectedWorldId! },
    { enabled: !!selectedWorldId }
  );
  const { data: worldLore = [] } = trpc.lore.byWorld.useQuery(
    { worldId: selectedWorldId! },
    { enabled: !!selectedWorldId }
  );
  const { data: eventHistory = [] } = trpc.eventBridge.history.useQuery({ limit: 20 });

  const createWorldMutation = trpc.world.create.useMutation({
    onSuccess: (world) => {
      refetchWorlds();
      setCreateWorldDialogOpen(false);
      setNewWorldName("");
      setNewWorldDescription("");
      setSelectedWorldId(world.id);
    },
  });

  const getEventTypeIcon = (type: string) => {
    switch (type) {
      case "economic": return <TrendingUp className="h-4 w-4" />;
      case "political": return <Globe className="h-4 w-4" />;
      case "natural": return <AlertTriangle className="h-4 w-4" />;
      case "technological": return <Zap className="h-4 w-4" />;
      case "social": return <Activity className="h-4 w-4" />;
      case "conflict": return <AlertTriangle className="h-4 w-4 text-red-500" />;
      case "discovery": return <Sparkles className="h-4 w-4 text-yellow-500" />;
      default: return <Calendar className="h-4 w-4" />;
    }
  };

  const getEventTypeColor = (type: string) => {
    switch (type) {
      case "economic": return "bg-blue-500/20 text-blue-400 border-blue-500/50";
      case "political": return "bg-purple-500/20 text-purple-400 border-purple-500/50";
      case "natural": return "bg-orange-500/20 text-orange-400 border-orange-500/50";
      case "technological": return "bg-cyan-500/20 text-cyan-400 border-cyan-500/50";
      case "social": return "bg-pink-500/20 text-pink-400 border-pink-500/50";
      case "conflict": return "bg-red-500/20 text-red-400 border-red-500/50";
      case "discovery": return "bg-yellow-500/20 text-yellow-400 border-yellow-500/50";
      default: return "bg-gray-500/20 text-gray-400 border-gray-500/50";
    }
  };

  return (
    <GameLayout>
      <div className="space-y-6">
        {/* Header */}
        <div className="flex items-center justify-between">
          <div>
            <h1 className="text-3xl font-bold flex items-center gap-2">
              <Globe className="h-8 w-8 text-primary" />
              World Events
            </h1>
            <p className="text-muted-foreground mt-1">
              Manage narrative worlds and their impact on the economy
            </p>
          </div>
          <Dialog open={createWorldDialogOpen} onOpenChange={setCreateWorldDialogOpen}>
            <DialogTrigger asChild>
              <Button>
                <Plus className="h-4 w-4 mr-2" />
                Create World
              </Button>
            </DialogTrigger>
            <DialogContent>
              <DialogHeader>
                <DialogTitle>Create New World</DialogTitle>
                <DialogDescription>
                  Define a narrative world that will influence the game economy.
                </DialogDescription>
              </DialogHeader>
              <div className="space-y-4 py-4">
                <div className="space-y-2">
                  <Label htmlFor="worldName">World Name</Label>
                  <Input
                    id="worldName"
                    value={newWorldName}
                    onChange={(e) => setNewWorldName(e.target.value)}
                    placeholder="Enter world name..."
                  />
                </div>
                <div className="space-y-2">
                  <Label htmlFor="genre">Genre</Label>
                  <Select value={newWorldGenre} onValueChange={setNewWorldGenre}>
                    <SelectTrigger>
                      <SelectValue placeholder="Select genre" />
                    </SelectTrigger>
                    <SelectContent>
                      <SelectItem value="modern">Modern</SelectItem>
                      <SelectItem value="historical">Historical</SelectItem>
                      <SelectItem value="futuristic">Futuristic</SelectItem>
                      <SelectItem value="fantasy">Fantasy</SelectItem>
                      <SelectItem value="steampunk">Steampunk</SelectItem>
                      <SelectItem value="cyberpunk">Cyberpunk</SelectItem>
                    </SelectContent>
                  </Select>
                </div>
                <div className="space-y-2">
                  <Label htmlFor="description">Description (Optional)</Label>
                  <Input
                    id="description"
                    value={newWorldDescription}
                    onChange={(e) => setNewWorldDescription(e.target.value)}
                    placeholder="Brief description..."
                  />
                </div>
                <Button
                  className="w-full"
                  onClick={() => createWorldMutation.mutate({
                    name: newWorldName,
                    genre: newWorldGenre as "modern" | "historical" | "futuristic" | "fantasy" | "steampunk" | "cyberpunk",
                    description: newWorldDescription || undefined,
                  })}
                  disabled={!newWorldName || createWorldMutation.isPending}
                >
                  {createWorldMutation.isPending ? "Creating..." : "Create World"}
                </Button>
              </div>
            </DialogContent>
          </Dialog>
        </div>

        <div className="grid grid-cols-1 lg:grid-cols-4 gap-6">
          {/* World List */}
          <Card className="lg:col-span-1">
            <CardHeader>
              <CardTitle>Worlds</CardTitle>
              <CardDescription>Select a world to view details</CardDescription>
            </CardHeader>
            <CardContent>
              <div className="space-y-2">
                {worlds?.length === 0 && (
                  <p className="text-muted-foreground text-sm text-center py-4">
                    No worlds yet. Create your first world!
                  </p>
                )}
                {worlds?.map((world) => (
                  <button
                    key={world.id}
                    onClick={() => setSelectedWorldId(world.id)}
                    className={`w-full p-3 rounded-lg border text-left transition-colors ${
                      selectedWorldId === world.id
                        ? "border-primary bg-primary/10"
                        : "border-border hover:border-primary/50"
                    }`}
                  >
                    <div className="flex items-center gap-3">
                      <div className="p-2 rounded-full bg-primary/20">
                        <Globe className="h-4 w-4" />
                      </div>
                      <div className="flex-1 min-w-0">
                        <p className="font-medium truncate">{world.name}</p>
                        <p className="text-xs text-muted-foreground capitalize">{world.genre}</p>
                      </div>
                    </div>
                  </button>
                ))}
              </div>
            </CardContent>
          </Card>

          {/* World Details */}
          <Card className="lg:col-span-3">
            {worldState ? (
              <Tabs defaultValue="overview">
                <CardHeader>
                  <div className="flex items-center justify-between">
                    <div>
                      <CardTitle className="flex items-center gap-2">
                        <Globe className="h-5 w-5" />
                        {worldState?.name}
                      </CardTitle>
                      <CardDescription className="capitalize">
                        {worldState?.genre} • Tech Level: {worldState?.technologyLevel || 'Unknown'}/100
                      </CardDescription>
                    </div>
                    <TabsList>
                      <TabsTrigger value="overview">Overview</TabsTrigger>
                      <TabsTrigger value="events">Events</TabsTrigger>
                      <TabsTrigger value="economy">Economy</TabsTrigger>
                      <TabsTrigger value="lore">Lore</TabsTrigger>
                    </TabsList>
                  </div>
                </CardHeader>
                <CardContent>
                  <TabsContent value="overview" className="mt-0">
                    <div className="space-y-6">
                      {/* World Info */}
                      <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
                        <Card>
                          <CardContent className="pt-4">
                            <div className="text-center">
                              <p className="text-2xl font-bold text-primary">
                                {(worldState as any)?.activeEvents?.length || 0}
                              </p>
                              <p className="text-xs text-muted-foreground">Active Events</p>
                            </div>
                          </CardContent>
                        </Card>
                        <Card>
                          <CardContent className="pt-4">
                            <div className="text-center">
                              <p className="text-2xl font-bold">
                                {worldState?.technologyLevel || 'Unknown'}
                              </p>
                              <p className="text-xs text-muted-foreground">Tech Level</p>
                            </div>
                          </CardContent>
                        </Card>
                        <Card>
                          <CardContent className="pt-4">
                            <div className="text-center">
                              <p className="text-2xl font-bold capitalize">
                                {(worldState as any)?.economicSystem || 'Unknown'}
                              </p>
                              <p className="text-xs text-muted-foreground">Economy</p>
                            </div>
                          </CardContent>
                        </Card>
                        <Card>
                          <CardContent className="pt-4">
                            <div className="text-center">
                              <p className="text-2xl font-bold">
                                {100}%
                              </p>
                              <p className="text-xs text-muted-foreground">Price Modifier</p>
                            </div>
                          </CardContent>
                        </Card>
                      </div>

                      {/* Description */}
                      {worldState?.description && (
                        <Card>
                          <CardContent className="pt-4">
                            <p className="text-sm">{worldState?.description}</p>
                          </CardContent>
                        </Card>
                      )}

                      {/* Active Events Summary */}
                      <div>
                        <h3 className="font-semibold mb-3 flex items-center gap-2">
                          <Zap className="h-4 w-4" />
                          Active Events
                        </h3>
                        {((worldState as any)?.activeEvents?.length || 0) === 0 ? (
                          <p className="text-muted-foreground text-sm">No active events</p>
                        ) : (
                          <div className="grid grid-cols-1 md:grid-cols-2 gap-3">
                            {((worldState as any)?.activeEvents || []).slice(0, 4).map((event: any) => (
                              <Card key={event.id} className={`border ${getEventTypeColor(event.type)}`}>
                                <CardContent className="pt-4">
                                  <div className="flex items-start gap-3">
                                    {getEventTypeIcon(event.type)}
                                    <div>
                                      <p className="font-medium">{event.name}</p>
                                      <p className="text-xs text-muted-foreground capitalize">
                                        {event.type} • Importance: {event.importance}/100
                                      </p>
                                    </div>
                                  </div>
                                </CardContent>
                              </Card>
                            ))}
                          </div>
                        )}
                      </div>
                    </div>
                  </TabsContent>

                  <TabsContent value="events" className="mt-0">
                    <div className="space-y-4">
                      <h3 className="font-semibold flex items-center gap-2">
                        <Calendar className="h-4 w-4" />
                        World Events
                      </h3>
                      {worldEvents?.length === 0 && (
                        <p className="text-muted-foreground text-center py-8">
                          No events in this world yet
                        </p>
                      )}
                      <div className="space-y-3">
                        {worldEvents?.map((event: any) => (
                          <Card key={event.id} className={`border ${getEventTypeColor(event.type)}`}>
                            <CardContent className="pt-4">
                              <div className="flex items-start justify-between">
                                <div className="flex items-start gap-3">
                                  {getEventTypeIcon(event.type)}
                                  <div>
                                    <p className="font-medium">{event.name}</p>
                                    <p className="text-sm text-muted-foreground mt-1">
                                      {event.description}
                                    </p>
                                    <div className="flex gap-2 mt-2">
                                      <Badge variant="outline" className="capitalize">
                                        {event.type}
                                      </Badge>
                                      <Badge variant="secondary">
                                        Turn {event.startTurn}{event.endTurn ? ` - ${event.endTurn}` : "+"}
                                      </Badge>
                                    </div>
                                  </div>
                                </div>
                                <Badge variant={event.isActive ? "default" : "secondary"}>
                                  {event.isActive ? "Active" : "Ended"}
                                </Badge>
                              </div>
                            </CardContent>
                          </Card>
                        ))}
                      </div>
                    </div>
                  </TabsContent>

                  <TabsContent value="economy" className="mt-0">
                    <div className="space-y-6">
                      {/* Economic Indicators */}
                      <div>
                        <h3 className="font-semibold mb-4 flex items-center gap-2">
                          <TrendingUp className="h-4 w-4" />
                          Economic Indicators
                        </h3>
                        <div className="grid grid-cols-2 md:grid-cols-5 gap-4">
                          <Card>
                            <CardContent className="pt-4">
                              <div className="text-center">
                                <p className={`text-2xl font-bold ${((worldState as any)?.economicIndicators || {}).marketHealth >= 50 ? "text-green-400" : "text-red-400"}`}>
                                  {((worldState as any)?.economicIndicators || {}).marketHealth}
                                </p>
                                <p className="text-xs text-muted-foreground">Market Health</p>
                              </div>
                            </CardContent>
                          </Card>
                          <Card>
                            <CardContent className="pt-4">
                              <div className="text-center">
                                <p className={`text-2xl font-bold flex items-center justify-center gap-1 ${((worldState as any)?.economicIndicators || {}).inflation > 5 ? "text-red-400" : ((worldState as any)?.economicIndicators || {}).inflation < 0 ? "text-blue-400" : "text-green-400"}`}>
                                  {((worldState as any)?.economicIndicators || {}).inflation > 0 ? <TrendingUp className="h-4 w-4" /> : <TrendingDown className="h-4 w-4" />}
                                  {((worldState as any)?.economicIndicators || {}).inflation}%
                                </p>
                                <p className="text-xs text-muted-foreground">Inflation</p>
                              </div>
                            </CardContent>
                          </Card>
                          <Card>
                            <CardContent className="pt-4">
                              <div className="text-center">
                                <p className="text-2xl font-bold">
                                  {((worldState as any)?.economicIndicators || {}).unemployment}%
                                </p>
                                <p className="text-xs text-muted-foreground">Unemployment</p>
                              </div>
                            </CardContent>
                          </Card>
                          <Card>
                            <CardContent className="pt-4">
                              <div className="text-center">
                                <p className={`text-2xl font-bold ${((worldState as any)?.economicIndicators || {}).consumerConfidence >= 50 ? "text-green-400" : "text-red-400"}`}>
                                  {((worldState as any)?.economicIndicators || {}).consumerConfidence}
                                </p>
                                <p className="text-xs text-muted-foreground">Consumer Confidence</p>
                              </div>
                            </CardContent>
                          </Card>
                          <Card>
                            <CardContent className="pt-4">
                              <div className="text-center">
                                <p className="text-2xl font-bold">
                                  {((worldState as any)?.economicIndicators || {}).tradeVolume.toLocaleString()}
                                </p>
                                <p className="text-xs text-muted-foreground">Trade Volume</p>
                              </div>
                            </CardContent>
                          </Card>
                        </div>
                      </div>

                      {/* Market Modifiers */}
                      <div>
                        <h3 className="font-semibold mb-4 flex items-center gap-2">
                          <Activity className="h-4 w-4" />
                          Global Market Modifiers
                        </h3>
                        <div className="grid grid-cols-3 gap-4">
                          <Card>
                            <CardContent className="pt-4">
                              <div className="text-center">
                                <p className={`text-2xl font-bold ${((worldState as any)?.marketModifiers || {}).globalPriceMultiplier > 1 ? "text-red-400" : ((worldState as any)?.marketModifiers || {}).globalPriceMultiplier < 1 ? "text-green-400" : ""}`}>
                                  {100}%
                                </p>
                                <p className="text-xs text-muted-foreground">Price Multiplier</p>
                              </div>
                            </CardContent>
                          </Card>
                          <Card>
                            <CardContent className="pt-4">
                              <div className="text-center">
                                <p className={`text-2xl font-bold ${((worldState as any)?.marketModifiers || {}).globalDemandMultiplier > 1 ? "text-green-400" : ((worldState as any)?.marketModifiers || {}).globalDemandMultiplier < 1 ? "text-red-400" : ""}`}>
                                  {(((worldState as any)?.marketModifiers || {}).globalDemandMultiplier * 100).toFixed(0)}%
                                </p>
                                <p className="text-xs text-muted-foreground">Demand Multiplier</p>
                              </div>
                            </CardContent>
                          </Card>
                          <Card>
                            <CardContent className="pt-4">
                              <div className="text-center">
                                <p className={`text-2xl font-bold ${((worldState as any)?.marketModifiers || {}).globalSupplyMultiplier > 1 ? "text-green-400" : ((worldState as any)?.marketModifiers || {}).globalSupplyMultiplier < 1 ? "text-red-400" : ""}`}>
                                  {(((worldState as any)?.marketModifiers || {}).globalSupplyMultiplier * 100).toFixed(0)}%
                                </p>
                                <p className="text-xs text-muted-foreground">Supply Multiplier</p>
                              </div>
                            </CardContent>
                          </Card>
                        </div>
                      </div>
                    </div>
                  </TabsContent>

                  <TabsContent value="lore" className="mt-0">
                    <div className="space-y-4">
                      <h3 className="font-semibold flex items-center gap-2">
                        <BookOpen className="h-4 w-4" />
                        World Lore
                      </h3>
                      {worldLore?.length === 0 && (
                        <p className="text-muted-foreground text-center py-8">
                          No lore entries yet
                        </p>
                      )}
                      <div className="space-y-3">
                        {worldLore?.map((entry: any) => (
                          <Card key={entry.id}>
                            <CardContent className="pt-4">
                              <div className="flex items-start justify-between mb-2">
                                <h4 className="font-medium">{entry.title}</h4>
                                <Badge variant="outline" className="capitalize">
                                  {entry.category}
                                </Badge>
                              </div>
                              <p className="text-sm text-muted-foreground">{entry.content}</p>
                              {entry.tags && (
                                <div className="flex gap-1 mt-2">
                                  {(entry.tags as string[]).map((tag, i) => (
                                    <Badge key={i} variant="secondary" className="text-xs">
                                      {tag}
                                    </Badge>
                                  ))}
                                </div>
                              )}
                            </CardContent>
                          </Card>
                        ))}
                      </div>
                    </div>
                  </TabsContent>
                </CardContent>
              </Tabs>
            ) : (
              <CardContent className="flex flex-col items-center justify-center py-16">
                <Globe className="h-16 w-16 text-muted-foreground mb-4" />
                <p className="text-muted-foreground">Select a world to view details</p>
              </CardContent>
            )}
          </Card>
        </div>

        {/* Event Propagation History */}
        <Card>
          <CardHeader>
            <CardTitle className="flex items-center gap-2">
              <Activity className="h-5 w-5" />
              Event Propagation History
            </CardTitle>
            <CardDescription>
              Recent cross-system event propagations between business and narrative
            </CardDescription>
          </CardHeader>
          <CardContent>
            {eventHistory?.length === 0 ? (
              <p className="text-muted-foreground text-center py-4">
                No event propagations recorded yet
              </p>
            ) : (
              <div className="space-y-2">
                {eventHistory?.slice(0, 10).map((event: any) => (
                  <div key={event.id} className="flex items-center justify-between p-3 rounded-lg border">
                    <div className="flex items-center gap-3">
                      <Badge variant={event.sourceType === "business" ? "default" : "secondary"}>
                        {event.sourceType}
                      </Badge>
                      <span className="text-muted-foreground">→</span>
                      <Badge variant={event.targetType === "business" ? "default" : "secondary"}>
                        {event.targetType}
                      </Badge>
                    </div>
                    <div className="flex items-center gap-2">
                      <span className="text-sm text-muted-foreground">
                        {event.sourceEventType} → {event.targetEventType}
                      </span>
                      <Badge variant={event.isProcessed ? "outline" : "default"}>
                        {event.isProcessed ? "Processed" : "Pending"}
                      </Badge>
                    </div>
                  </div>
                ))}
              </div>
            )}
          </CardContent>
        </Card>
      </div>
    </GameLayout>
  );
}
