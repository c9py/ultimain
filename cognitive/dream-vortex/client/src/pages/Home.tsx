import { useAuth } from "@/_core/hooks/useAuth";
import { Button } from "@/components/ui/button";
import { getLoginUrl } from "@/const";
import { useTheme } from "@/contexts/ThemeContext";
import {
  Building2,
  Factory,
  TrendingUp,
  Users,
  Globe,
  Zap,
  ChevronRight,
  Moon,
  Sun,
  Loader2,
} from "lucide-react";
import { useLocation } from "wouter";

export default function Home() {
  const { user, loading, isAuthenticated } = useAuth();
  const { theme, toggleTheme } = useTheme();
  const [, setLocation] = useLocation();

  if (loading) {
    return (
      <div className="min-h-screen flex items-center justify-center bg-background">
        <Loader2 className="w-8 h-8 animate-spin text-primary" />
      </div>
    );
  }

  const features = [
    {
      icon: Building2,
      title: "Build Your Empire",
      description:
        "Start with a small company and grow it into a multinational corporation. Manage offices, stores, factories, and more.",
    },
    {
      icon: Factory,
      title: "Production Chains",
      description:
        "Create complex supply chains from raw materials to finished products. Optimize production for maximum efficiency.",
    },
    {
      icon: TrendingUp,
      title: "Dynamic Markets",
      description:
        "Trade on realistic markets with prices driven by supply and demand. Find opportunities and outcompete rivals.",
    },
    {
      icon: Users,
      title: "Multiplayer Economy",
      description:
        "Compete and collaborate with thousands of players in a persistent world economy.",
    },
    {
      icon: Globe,
      title: "Global Operations",
      description:
        "Expand across multiple cities and countries. Each location has unique advantages and challenges.",
    },
    {
      icon: Zap,
      title: "Research & Innovation",
      description:
        "Invest in R&D to unlock new technologies and gain competitive advantages in the market.",
    },
  ];

  return (
    <div className="min-h-screen bg-background">
      {/* Header */}
      <header className="fixed top-0 left-0 right-0 z-50 bg-background/80 backdrop-blur-md border-b border-border">
        <div className="container mx-auto px-4 h-16 flex items-center justify-between">
          <div className="flex items-center gap-2">
            <div className="w-10 h-10 rounded-lg bg-primary flex items-center justify-center">
              <Building2 className="w-6 h-6 text-primary-foreground" />
            </div>
            <span className="text-xl font-bold">Virtunomics</span>
          </div>

          <div className="flex items-center gap-4">
            <Button
              variant="ghost"
              size="icon"
              onClick={toggleTheme}
              className="rounded-full"
            >
              {theme === "dark" ? (
                <Sun className="w-5 h-5" />
              ) : (
                <Moon className="w-5 h-5" />
              )}
            </Button>

            {isAuthenticated ? (
              <Button onClick={() => setLocation("/dashboard")}>
                Go to Dashboard
                <ChevronRight className="w-4 h-4 ml-1" />
              </Button>
            ) : (
              <Button onClick={() => (window.location.href = getLoginUrl())}>
                Sign In
              </Button>
            )}
          </div>
        </div>
      </header>

      {/* Hero Section */}
      <section className="pt-32 pb-20 hero-gradient">
        <div className="container mx-auto px-4">
          <div className="max-w-4xl mx-auto text-center">
            <h1 className="text-5xl md:text-6xl font-bold mb-6 bg-gradient-to-r from-primary via-blue-400 to-primary bg-clip-text text-transparent">
              Business Simulation Game
            </h1>
            <p className="text-xl text-muted-foreground mb-8 max-w-2xl mx-auto">
              Build your business empire in the most realistic economic
              simulation. Manage companies, trade on dynamic markets, and compete
              with entrepreneurs worldwide.
            </p>
            <div className="flex flex-col sm:flex-row gap-4 justify-center">
              {isAuthenticated ? (
                <Button
                  size="lg"
                  className="text-lg px-8 glow-primary"
                  onClick={() => setLocation("/dashboard")}
                >
                  Enter Game
                  <ChevronRight className="w-5 h-5 ml-2" />
                </Button>
              ) : (
                <Button
                  size="lg"
                  className="text-lg px-8 glow-primary"
                  onClick={() => (window.location.href = getLoginUrl())}
                >
                  Start Playing Free
                  <ChevronRight className="w-5 h-5 ml-2" />
                </Button>
              )}
              <Button
                size="lg"
                variant="outline"
                className="text-lg px-8"
                onClick={() => setLocation("/leaderboard")}
              >
                View Leaderboard
              </Button>
            </div>
          </div>

          {/* Stats */}
          <div className="mt-16 grid grid-cols-2 md:grid-cols-4 gap-6 max-w-4xl mx-auto">
            {[
              { value: "200+", label: "Industries" },
              { value: "12", label: "Global Cities" },
              { value: "∞", label: "Possibilities" },
              { value: "24/7", label: "Live Economy" },
            ].map((stat) => (
              <div key={stat.label} className="text-center">
                <div className="text-3xl md:text-4xl font-bold text-primary">
                  {stat.value}
                </div>
                <div className="text-sm text-muted-foreground mt-1">
                  {stat.label}
                </div>
              </div>
            ))}
          </div>
        </div>
      </section>

      {/* Features Section */}
      <section className="py-20 bg-card/50">
        <div className="container mx-auto px-4">
          <div className="text-center mb-12">
            <h2 className="text-3xl font-bold mb-4">
              Everything You Need to Build a Business Empire
            </h2>
            <p className="text-muted-foreground max-w-2xl mx-auto">
              Experience the most comprehensive business simulation with realistic
              mechanics and endless strategic possibilities.
            </p>
          </div>

          <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-6 max-w-6xl mx-auto">
            {features.map((feature) => (
              <div
                key={feature.title}
                className="game-card group hover:border-primary/50 transition-colors"
              >
                <div className="w-12 h-12 rounded-lg bg-primary/10 flex items-center justify-center mb-4 group-hover:bg-primary/20 transition-colors">
                  <feature.icon className="w-6 h-6 text-primary" />
                </div>
                <h3 className="text-lg font-semibold mb-2">{feature.title}</h3>
                <p className="text-muted-foreground text-sm">
                  {feature.description}
                </p>
              </div>
            ))}
          </div>
        </div>
      </section>

      {/* CTA Section */}
      <section className="py-20">
        <div className="container mx-auto px-4">
          <div className="max-w-3xl mx-auto text-center bg-gradient-to-br from-primary/10 to-primary/5 rounded-2xl p-12 border border-primary/20">
            <h2 className="text-3xl font-bold mb-4">Ready to Become a Tycoon?</h2>
            <p className="text-muted-foreground mb-8">
              Join thousands of entrepreneurs building their virtual empires. Start
              with $1,000,000 and see how far you can go.
            </p>
            {isAuthenticated ? (
              <Button
                size="lg"
                className="text-lg px-8"
                onClick={() => setLocation("/dashboard")}
              >
                Continue to Dashboard
                <ChevronRight className="w-5 h-5 ml-2" />
              </Button>
            ) : (
              <Button
                size="lg"
                className="text-lg px-8"
                onClick={() => (window.location.href = getLoginUrl())}
              >
                Create Free Account
                <ChevronRight className="w-5 h-5 ml-2" />
              </Button>
            )}
          </div>
        </div>
      </section>

      {/* Footer */}
      <footer className="py-8 border-t border-border">
        <div className="container mx-auto px-4">
          <div className="flex flex-col md:flex-row items-center justify-between gap-4">
            <div className="flex items-center gap-2">
              <div className="w-8 h-8 rounded-lg bg-primary flex items-center justify-center">
                <Building2 className="w-4 h-4 text-primary-foreground" />
              </div>
              <span className="font-semibold">Virtunomics</span>
            </div>
            <p className="text-sm text-muted-foreground">
              Business Simulation Game - Build Your Empire
            </p>
          </div>
        </div>
      </footer>
    </div>
  );
}
