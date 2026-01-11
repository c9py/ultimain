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
import { Progress } from "../components/ui/progress";
import { Dialog, DialogContent, DialogDescription, DialogHeader, DialogTitle, DialogTrigger } from "../components/ui/dialog";
import { Brain, Heart, Target, Zap, Users, TrendingUp, Shield, Lightbulb, Activity, Plus, User } from "lucide-react";

export default function Agents() {
  const [selectedAgentId, setSelectedAgentId] = useState<number | null>(null);
  const [createDialogOpen, setCreateDialogOpen] = useState(false);
  const [newAgentName, setNewAgentName] = useState("");
  const [newAgentType, setNewAgentType] = useState<string>("employee");
  const [newAgentBio, setNewAgentBio] = useState("");

  const { data: agents, refetch: refetchAgents } = trpc.agent.byCompany.useQuery();
  const { data: selectedAgent, refetch: refetchSelectedAgent } = trpc.agent.byId.useQuery(
    { id: selectedAgentId! },
    { enabled: !!selectedAgentId }
  );
  const { data: agentHistory = [] } = trpc.agent.history.useQuery(
    { agentId: selectedAgentId!, limit: 10 },
    { enabled: !!selectedAgentId }
  );
  const { data: agentPersonality } = trpc.personality.get.useQuery(
    { agentId: selectedAgentId! },
    { enabled: !!selectedAgentId }
  );
  const makeDecisionMutation = trpc.agent.makeDecision.useMutation();

  const createAgentMutation = trpc.agent.create.useMutation({
    onSuccess: () => {
      refetchAgents();
      setCreateDialogOpen(false);
      setNewAgentName("");
      setNewAgentBio("");
    },
  });

  const getPersonalityColor = (value: number) => {
    if (value >= 70) return "text-green-400";
    if (value >= 40) return "text-yellow-400";
    return "text-red-400";
  };

  const getEmotionColor = (value: number) => {
    if (value >= 60) return "bg-green-500";
    if (value >= 40) return "bg-yellow-500";
    return "bg-red-500";
  };

  const agentTypeIcons: Record<string, React.ReactNode> = {
    customer: <Users className="h-4 w-4" />,
    supplier: <TrendingUp className="h-4 w-4" />,
    employee: <User className="h-4 w-4" />,
    partner: <Shield className="h-4 w-4" />,
    investor: <Zap className="h-4 w-4" />,
    competitor: <Target className="h-4 w-4" />,
  };

  return (
    <GameLayout>
      <div className="space-y-6">
        {/* Header */}
        <div className="flex items-center justify-between">
          <div>
            <h1 className="text-3xl font-bold flex items-center gap-2">
              <Brain className="h-8 w-8 text-primary" />
              AI Agents
            </h1>
            <p className="text-muted-foreground mt-1">
              Manage AI-driven entities with unique personalities and behaviors
            </p>
          </div>
          <Dialog open={createDialogOpen} onOpenChange={setCreateDialogOpen}>
            <DialogTrigger asChild>
              <Button>
                <Plus className="h-4 w-4 mr-2" />
                Create Agent
              </Button>
            </DialogTrigger>
            <DialogContent>
              <DialogHeader>
                <DialogTitle>Create New Agent</DialogTitle>
                <DialogDescription>
                  Create an AI agent with a unique personality generated from the Big Five model.
                </DialogDescription>
              </DialogHeader>
              <div className="space-y-4 py-4">
                <div className="space-y-2">
                  <Label htmlFor="name">Agent Name</Label>
                  <Input
                    id="name"
                    value={newAgentName}
                    onChange={(e) => setNewAgentName(e.target.value)}
                    placeholder="Enter agent name..."
                  />
                </div>
                <div className="space-y-2">
                  <Label htmlFor="type">Agent Type</Label>
                  <Select value={newAgentType} onValueChange={setNewAgentType}>
                    <SelectTrigger>
                      <SelectValue placeholder="Select type" />
                    </SelectTrigger>
                    <SelectContent>
                      <SelectItem value="customer">Customer</SelectItem>
                      <SelectItem value="supplier">Supplier</SelectItem>
                      <SelectItem value="employee">Employee</SelectItem>
                      <SelectItem value="partner">Partner</SelectItem>
                      <SelectItem value="investor">Investor</SelectItem>
                      <SelectItem value="competitor">Competitor</SelectItem>
                    </SelectContent>
                  </Select>
                </div>
                <div className="space-y-2">
                  <Label htmlFor="bio">Bio (Optional)</Label>
                  <Input
                    id="bio"
                    value={newAgentBio}
                    onChange={(e) => setNewAgentBio(e.target.value)}
                    placeholder="Brief description..."
                  />
                </div>
                <Button
                  className="w-full"
                  onClick={() => createAgentMutation.mutate({
                    name: newAgentName,
                    type: newAgentType as "customer" | "supplier" | "employee" | "partner" | "investor" | "competitor",
                    personaId: 1, // Default persona
                    cityId: 1, // Default city
                  })}
                  disabled={!newAgentName || createAgentMutation.isPending}
                >
                  {createAgentMutation.isPending ? "Creating..." : "Create Agent"}
                </Button>
              </div>
            </DialogContent>
          </Dialog>
        </div>

        <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
          {/* Agent List */}
          <Card className="lg:col-span-1">
            <CardHeader>
              <CardTitle>Your Agents</CardTitle>
              <CardDescription>Select an agent to view details</CardDescription>
            </CardHeader>
            <CardContent>
              <div className="space-y-2">
                {agents?.length === 0 && (
                  <p className="text-muted-foreground text-sm text-center py-4">
                    No agents yet. Create your first agent!
                  </p>
                )}
                {agents?.map((agent: any) => (
                  <button
                    key={agent.id}
                    onClick={() => setSelectedAgentId(agent.id)}
                    className={`w-full p-3 rounded-lg border text-left transition-colors ${
                      selectedAgentId === agent.id
                        ? "border-primary bg-primary/10"
                        : "border-border hover:border-primary/50"
                    }`}
                  >
                    <div className="flex items-center gap-3">
                      <div className="p-2 rounded-full bg-primary/20">
                        {agentTypeIcons[agent.type] || <User className="h-4 w-4" />}
                      </div>
                      <div className="flex-1 min-w-0">
                        <p className="font-medium truncate">{agent.name}</p>
                        <p className="text-xs text-muted-foreground capitalize">{agent.type}</p>
                      </div>
                      <Badge variant={agent.isActive ? "default" : "secondary"}>
                        {agent.isActive ? "Active" : "Inactive"}
                      </Badge>
                    </div>
                  </button>
                ))}
              </div>
            </CardContent>
          </Card>

          {/* Agent Details */}
          <Card className="lg:col-span-2">
            {selectedAgent ? (
              <Tabs defaultValue="personality">
                <CardHeader>
                  <div className="flex items-center justify-between">
                    <div>
                      <CardTitle className="flex items-center gap-2">
                        {agentTypeIcons[selectedAgent.type]}
                        {selectedAgent.name}
                      </CardTitle>
                      <CardDescription className="capitalize">
                        {selectedAgent.type} • Created {new Date(selectedAgent.createdAt).toLocaleDateString()}
                      </CardDescription>
                    </div>
                    <TabsList>
                      <TabsTrigger value="personality">
                        <Brain className="h-4 w-4 mr-1" />
                        Personality
                      </TabsTrigger>
                      <TabsTrigger value="emotions">
                        <Heart className="h-4 w-4 mr-1" />
                        Emotions
                      </TabsTrigger>
                      <TabsTrigger value="decisions">
                        <Lightbulb className="h-4 w-4 mr-1" />
                        Decisions
                      </TabsTrigger>
                    </TabsList>
                  </div>
                </CardHeader>
                <CardContent>
                  <TabsContent value="personality" className="mt-0">
                    <div className="space-y-6">
                      {/* Big Five Traits */}
                      <div>
                        <h3 className="font-semibold mb-4 flex items-center gap-2">
                          <Brain className="h-4 w-4" />
                          Big Five Personality Traits
                        </h3>
                        <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                          {[
                            { name: "Openness", value: agentPersonality?.openness ?? 50, desc: "Curiosity and creativity" },
                            { name: "Conscientiousness", value: agentPersonality?.conscientiousness ?? 50, desc: "Organization and reliability" },
                            { name: "Extraversion", value: agentPersonality?.extraversion ?? 50, desc: "Sociability and energy" },
                            { name: "Agreeableness", value: agentPersonality?.agreeableness ?? 50, desc: "Cooperation and trust" },
                            { name: "Neuroticism", value: agentPersonality?.neuroticism ?? 50, desc: "Emotional sensitivity" },
                          ].map((trait) => (
                            <div key={trait.name} className="space-y-2">
                              <div className="flex justify-between text-sm">
                                <span>{trait.name}</span>
                                <span className={getPersonalityColor(trait.value)}>{trait.value}/100</span>
                              </div>
                              <Progress value={trait.value} className="h-2" />
                              <p className="text-xs text-muted-foreground">{trait.desc}</p>
                            </div>
                          ))}
                        </div>
                      </div>

                      {/* Behavioral Tendencies */}
                      <div>
                        <h3 className="font-semibold mb-4 flex items-center gap-2">
                          <Activity className="h-4 w-4" />
                          Behavioral Tendencies
                        </h3>
                        <div className="grid grid-cols-2 md:grid-cols-5 gap-4">
                          {[
                            { name: "Impulsiveness", value: agentPersonality?.impulsiveness ?? 50 },
                            { name: "Risk Taking", value: agentPersonality?.riskTaking ?? 50 },
                            { name: "Empathy", value: agentPersonality?.empathy ?? 50 },
                            { name: "Leadership", value: agentPersonality?.leadership ?? 50 },
                            { name: "Independence", value: agentPersonality?.independence ?? 50 },
                          ].map((trait) => (
                            <div key={trait.name} className="text-center p-3 rounded-lg bg-muted/50">
                              <p className={`text-2xl font-bold ${getPersonalityColor(trait.value)}`}>
                                {trait.value}
                              </p>
                              <p className="text-xs text-muted-foreground">{trait.name}</p>
                            </div>
                          ))}
                        </div>
                      </div>

                      {/* Motivations */}
                      {false && (
                        <div>
                          <h3 className="font-semibold mb-4 flex items-center gap-2">
                            <Target className="h-4 w-4" />
                            Motivations
                          </h3>
                          <div className="space-y-3">
                           {([] as any[]).map((motivation: any) => (                              <div key={motivation.id} className="p-3 rounded-lg border">
                                <div className="flex items-center justify-between mb-2">
                                  <Badge variant="outline" className="capitalize">
                                    {motivation.type.replace("_", " ")}
                                  </Badge>
                                  <span className="text-sm text-muted-foreground">
                                    Priority: {motivation.priority}/100
                                  </span>
                                </div>
                                <p className="text-sm">{motivation.description}</p>
                                <Progress value={motivation.progress} className="h-1 mt-2" />
                              </div>
                            ))}
                          </div>
                        </div>
                      )}
                    </div>
                  </TabsContent>

                  <TabsContent value="emotions" className="mt-0">
                    {selectedAgent ? (
                      <div className="space-y-6">
                        {/* Core Emotions */}
                        <div>
                          <h3 className="font-semibold mb-4 flex items-center gap-2">
                            <Heart className="h-4 w-4" />
                            Current Emotional State
                          </h3>
                          <div className="grid grid-cols-2 md:grid-cols-3 gap-4">
                            {[
                              { name: "Happiness", value: selectedAgent?.happiness || 50 },
                              { name: "Satisfaction", value: selectedAgent?.satisfaction || 50 },
                              { name: "Stress", value: selectedAgent?.stress || 50 },
                              { name: "Loyalty", value: selectedAgent?.loyalty || 50 },
                              { name: "Trust", value: selectedAgent?.trust || 50 },
                              { name: "Financial Need", value: selectedAgent?.financialNeed || 50 },
                            ].map((emotion) => (
                              <div key={emotion.name} className="space-y-2">
                                <div className="flex justify-between text-sm">
                                  <span>{emotion.name}</span>
                                  <span>{emotion.value}/100</span>
                                </div>
                                <div className="h-2 bg-muted rounded-full overflow-hidden">
                                  <div
                                    className={`h-full ${getEmotionColor(emotion.name === "Stress" || emotion.name === "Anger" || emotion.name === "Fear" ? 100 - emotion.value : emotion.value)}`}
                                    style={{ width: `${emotion.value}%` }}
                                  />
                                </div>
                              </div>
                            ))}
                          </div>
                        </div>

                        {/* Psychological Needs */}
                        <div>
                          <h3 className="font-semibold mb-4 flex items-center gap-2">
                            <Zap className="h-4 w-4" />
                            Psychological Needs Fulfillment
                          </h3>
                          <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                            {[
                              { name: "Financial Security", value: selectedAgent?.financialNeed || 50 },
                              { name: "Recognition", value: selectedAgent?.recognitionNeed || 50 },
                              { name: "Autonomy", value: selectedAgent?.autonomyNeed || 50 },
                              { name: "Belonging", value: selectedAgent?.socialNeed || 50 },
                              { name: "Security", value: selectedAgent?.securityNeed || 50 },
                            ].map((need) => (
                              <div key={need.name} className="space-y-2">
                                <div className="flex justify-between text-sm">
                                  <span>{need.name}</span>
                                  <span className={getPersonalityColor(need.value)}>{need.value}%</span>
                                </div>
                                <Progress value={need.value} className="h-2" />
                              </div>
                            ))}
                          </div>
                        </div>

                        {/* Overall State */}
                        <div className="grid grid-cols-2 gap-4">
                          <Card>
                            <CardContent className="pt-6">
                              <div className="text-center">
                                <p className={`text-4xl font-bold ${getPersonalityColor(selectedAgent?.happiness || 50)}`}>
                                  {selectedAgent?.happiness || 50}
                                </p>
                                <p className="text-sm text-muted-foreground">Overall Mood</p>
                              </div>
                            </CardContent>
                          </Card>
                          <Card>
                            <CardContent className="pt-6">
                              <div className="text-center">
                                <p className={`text-4xl font-bold ${getPersonalityColor(100 - (selectedAgent?.stress || 50))}`}>
                                  {selectedAgent?.stress || 50}
                                </p>
                                <p className="text-sm text-muted-foreground">Stress Level</p>
                              </div>
                            </CardContent>
                          </Card>
                        </div>
                      </div>
                    ) : (
                      <p className="text-muted-foreground text-center py-8">
                        No emotional state data available
                      </p>
                    )}
                  </TabsContent>

                  <TabsContent value="decisions" className="mt-0">
                    <div className="space-y-4">
                      <h3 className="font-semibold flex items-center gap-2">
                        <Lightbulb className="h-4 w-4" />
                        Agent History & Decisions
                      </h3>

                      {/* Quick Decision Test */}
                      <Card className="bg-muted/30">
                        <CardContent className="pt-4">
                          <p className="text-sm text-muted-foreground mb-3">
                            Test agent decision-making with a sample scenario
                          </p>
                          <Button
                            size="sm"
                            disabled={makeDecisionMutation.isPending}
                            onClick={async () => {
                              if (!selectedAgentId) return;
                              try {
                                const result = await makeDecisionMutation.mutateAsync({
                                  agentId: selectedAgentId,
                                  context: {
                                    type: "trade",
                                    situation: "A new supplier is offering materials at 20% below market price, but they're a new company with no track record.",
                                    options: [
                                      { id: "accept", description: "Accept the deal", expectedOutcome: "Save money but risk quality", riskLevel: 65, potentialReward: 70, requiresCooperation: false, requiresConflict: false },
                                      { id: "negotiate", description: "Negotiate for smaller trial order", expectedOutcome: "Test relationship before committing", riskLevel: 30, potentialReward: 50, requiresCooperation: true, requiresConflict: false },
                                      { id: "reject", description: "Decline and stay with current supplier", expectedOutcome: "Maintain stability", riskLevel: 10, potentialReward: 20, requiresCooperation: false, requiresConflict: false },
                                    ],
                                  },
                                });
                                refetchSelectedAgent();
                                alert(`Decision: ${result.chosenOption.description}\n\nReasoning: ${result.reasoning}\n\nConfidence: ${result.confidence}%`);
                              } catch (err) {
                                console.error(err);
                              }
                            }}
                          >
                            {makeDecisionMutation.isPending ? "Thinking..." : "Run Sample Decision"}
                          </Button>
                        </CardContent>
                      </Card>

                      {agentHistory?.length === 0 ? (
                        <p className="text-muted-foreground text-center py-8">
                          No history recorded yet
                        </p>
                      ) : (
                        agentHistory?.map((entry: any) => (
                          <Card key={entry.id}>
                            <CardContent className="pt-4">
                              <div className="flex items-center justify-between mb-2">
                                <Badge variant="outline">
                                  Record #{entry.id}
                                </Badge>
                                <span className="text-xs text-muted-foreground">
                                  {new Date(entry.recordedAt).toLocaleString()}
                                </span>
                              </div>
                              <p className="text-sm mb-2">{entry.notes || "State snapshot"}</p>
                              <div className="flex gap-4 text-xs text-muted-foreground">
                                <span>Happiness: {entry.happiness}</span>
                                <span>Satisfaction: {entry.satisfaction}</span>
                                <span>Stress: {entry.stress}</span>
                              </div>
                            </CardContent>
                          </Card>
                        ))
                      )}
                    </div>
                  </TabsContent>
                </CardContent>
              </Tabs>
            ) : (
              <CardContent className="flex flex-col items-center justify-center py-16">
                <Brain className="h-16 w-16 text-muted-foreground mb-4" />
                <p className="text-muted-foreground">Select an agent to view their profile</p>
              </CardContent>
            )}
          </Card>
        </div>
      </div>
    </GameLayout>
  );
}
