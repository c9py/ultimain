import { Toaster } from "@/components/ui/sonner";
import { TooltipProvider } from "@/components/ui/tooltip";
import NotFound from "@/pages/NotFound";
import { Route, Switch } from "wouter";
import ErrorBoundary from "./components/ErrorBoundary";
import { ThemeProvider } from "./contexts/ThemeContext";

// Pages
import Home from "./pages/Home";
import Dashboard from "./pages/Dashboard";
import CompanySetup from "./pages/CompanySetup";
import BusinessUnits from "./pages/BusinessUnits";
import UnitDetail from "./pages/UnitDetail";
import Market from "./pages/Market";
import Finance from "./pages/Finance";
import Leaderboard from "./pages/Leaderboard";
import Production from "./pages/Production";

function Router() {
  return (
    <Switch>
      <Route path="/" component={Home} />
      <Route path="/dashboard" component={Dashboard} />
      <Route path="/setup" component={CompanySetup} />
      <Route path="/units" component={BusinessUnits} />
      <Route path="/units/:id" component={UnitDetail} />
      <Route path="/market" component={Market} />
      <Route path="/finance" component={Finance} />
      <Route path="/leaderboard" component={Leaderboard} />
      <Route path="/production" component={Production} />
      <Route path="/404" component={NotFound} />
      <Route component={NotFound} />
    </Switch>
  );
}

function App() {
  return (
    <ErrorBoundary>
      <ThemeProvider
        defaultTheme="dark"
        switchable
      >
        <TooltipProvider>
          <Toaster />
          <Router />
        </TooltipProvider>
      </ThemeProvider>
    </ErrorBoundary>
  );
}

export default App;
