import { useAuth } from "@/_core/hooks/useAuth";
import GameLayout from "@/components/GameLayout";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow,
} from "@/components/ui/table";
import { getLoginUrl } from "@/const";
import { trpc } from "@/lib/trpc";
import {
  Award,
  Building2,
  Crown,
  Loader2,
  Medal,
  Trophy,
} from "lucide-react";

function LeaderboardContent() {
  const { data: companies, isLoading } = trpc.company.all.useQuery();
  const { data: myCompany } = trpc.company.mine.useQuery();

  const getRankIcon = (rank: number) => {
    switch (rank) {
      case 1:
        return <Crown className="w-5 h-5 text-yellow-500" />;
      case 2:
        return <Medal className="w-5 h-5 text-gray-400" />;
      case 3:
        return <Medal className="w-5 h-5 text-amber-600" />;
      default:
        return <span className="text-muted-foreground">#{rank}</span>;
    }
  };

  const getRankBadge = (rank: number) => {
    if (rank === 1) return "bg-yellow-500/20 text-yellow-500 border-yellow-500/30";
    if (rank === 2) return "bg-gray-400/20 text-gray-400 border-gray-400/30";
    if (rank === 3) return "bg-amber-600/20 text-amber-600 border-amber-600/30";
    return "bg-muted text-muted-foreground border-border";
  };

  return (
    <div className="space-y-6">
      {/* Header */}
      <div>
        <h1 className="text-3xl font-bold">Leaderboard</h1>
        <p className="text-muted-foreground">
          Top companies ranked by total assets
        </p>
      </div>

      {/* Top 3 Podium */}
      {companies && companies.length >= 3 && (
        <div className="grid md:grid-cols-3 gap-4">
          {/* Second Place */}
          <Card className="md:mt-8 border-gray-400/30">
            <CardContent className="pt-6 text-center">
              <div className="w-16 h-16 rounded-full bg-gray-400/20 flex items-center justify-center mx-auto mb-4">
                <Medal className="w-8 h-8 text-gray-400" />
              </div>
              <p className="text-sm text-muted-foreground">2nd Place</p>
              <h3 className="text-xl font-bold mt-1">{companies[1].name}</h3>
              <p className="text-lg font-semibold text-success mt-2">
                ${parseFloat(companies[1].cash).toLocaleString()}
              </p>
            </CardContent>
          </Card>

          {/* First Place */}
          <Card className="border-yellow-500/30 glow-gold">
            <CardContent className="pt-6 text-center">
              <div className="w-20 h-20 rounded-full bg-yellow-500/20 flex items-center justify-center mx-auto mb-4">
                <Crown className="w-10 h-10 text-yellow-500" />
              </div>
              <p className="text-sm text-yellow-500">1st Place</p>
              <h3 className="text-2xl font-bold mt-1">{companies[0].name}</h3>
              <p className="text-xl font-semibold text-success mt-2">
                ${parseFloat(companies[0].cash).toLocaleString()}
              </p>
            </CardContent>
          </Card>

          {/* Third Place */}
          <Card className="md:mt-12 border-amber-600/30">
            <CardContent className="pt-6 text-center">
              <div className="w-14 h-14 rounded-full bg-amber-600/20 flex items-center justify-center mx-auto mb-4">
                <Medal className="w-7 h-7 text-amber-600" />
              </div>
              <p className="text-sm text-muted-foreground">3rd Place</p>
              <h3 className="text-lg font-bold mt-1">{companies[2].name}</h3>
              <p className="text-lg font-semibold text-success mt-2">
                ${parseFloat(companies[2].cash).toLocaleString()}
              </p>
            </CardContent>
          </Card>
        </div>
      )}

      {/* My Ranking */}
      {myCompany && companies && (
        <Card className="border-primary/30">
          <CardHeader>
            <CardTitle className="flex items-center gap-2">
              <Award className="w-5 h-5 text-primary" />
              Your Ranking
            </CardTitle>
          </CardHeader>
          <CardContent>
            <div className="flex items-center justify-between">
              <div className="flex items-center gap-4">
                <div
                  className={`w-12 h-12 rounded-full flex items-center justify-center border ${getRankBadge(
                    companies.findIndex((c) => c.id === myCompany.id) + 1
                  )}`}
                >
                  {getRankIcon(
                    companies.findIndex((c) => c.id === myCompany.id) + 1
                  )}
                </div>
                <div>
                  <p className="font-bold text-lg">{myCompany.name}</p>
                  <p className="text-sm text-muted-foreground">
                    Rank #{companies.findIndex((c) => c.id === myCompany.id) + 1}{" "}
                    of {companies.length}
                  </p>
                </div>
              </div>
              <div className="text-right">
                <p className="text-2xl font-bold text-success">
                  ${parseFloat(myCompany.cash).toLocaleString()}
                </p>
                <p className="text-sm text-muted-foreground">Total Cash</p>
              </div>
            </div>
          </CardContent>
        </Card>
      )}

      {/* Full Rankings Table */}
      <Card>
        <CardHeader>
          <CardTitle>All Companies</CardTitle>
          <CardDescription>
            Complete ranking of all companies by cash holdings
          </CardDescription>
        </CardHeader>
        <CardContent>
          {isLoading ? (
            <div className="flex items-center justify-center py-12">
              <Loader2 className="w-8 h-8 animate-spin text-primary" />
            </div>
          ) : companies && companies.length > 0 ? (
            <Table>
              <TableHeader>
                <TableRow>
                  <TableHead className="w-16">Rank</TableHead>
                  <TableHead>Company</TableHead>
                  <TableHead className="text-right">Reputation</TableHead>
                  <TableHead className="text-right">Cash</TableHead>
                </TableRow>
              </TableHeader>
              <TableBody>
                {companies.map((company, index) => (
                  <TableRow
                    key={company.id}
                    className={
                      myCompany?.id === company.id ? "bg-primary/5" : ""
                    }
                  >
                    <TableCell>
                      <div
                        className={`w-8 h-8 rounded-full flex items-center justify-center ${getRankBadge(
                          index + 1
                        )}`}
                      >
                        {getRankIcon(index + 1)}
                      </div>
                    </TableCell>
                    <TableCell>
                      <div className="flex items-center gap-3">
                        <div className="w-10 h-10 rounded-lg bg-primary/10 flex items-center justify-center">
                          <Building2 className="w-5 h-5 text-primary" />
                        </div>
                        <div>
                          <p className="font-medium">
                            {company.name}
                            {myCompany?.id === company.id && (
                              <span className="ml-2 text-xs text-primary">
                                (You)
                              </span>
                            )}
                          </p>
                          <p className="text-xs text-muted-foreground">
                            Founded{" "}
                            {new Date(company.founded).toLocaleDateString()}
                          </p>
                        </div>
                      </div>
                    </TableCell>
                    <TableCell className="text-right">
                      <div className="flex items-center justify-end gap-2">
                        <div className="w-16 h-2 bg-muted rounded-full overflow-hidden">
                          <div
                            className="h-full bg-primary"
                            style={{ width: `${company.reputation}%` }}
                          />
                        </div>
                        <span className="text-sm tabular-nums">
                          {company.reputation}
                        </span>
                      </div>
                    </TableCell>
                    <TableCell className="text-right">
                      <span className="font-semibold tabular-nums text-success">
                        ${parseFloat(company.cash).toLocaleString()}
                      </span>
                    </TableCell>
                  </TableRow>
                ))}
              </TableBody>
            </Table>
          ) : (
            <div className="text-center py-12">
              <Trophy className="w-12 h-12 mx-auto text-muted-foreground mb-4" />
              <p className="text-muted-foreground">No companies yet</p>
              <p className="text-sm text-muted-foreground mt-1">
                Be the first to create a company and claim the top spot!
              </p>
            </div>
          )}
        </CardContent>
      </Card>
    </div>
  );
}

export default function Leaderboard() {
  const { isAuthenticated, loading } = useAuth();

  if (loading) {
    return (
      <div className="min-h-screen flex items-center justify-center bg-background">
        <Loader2 className="w-8 h-8 animate-spin text-primary" />
      </div>
    );
  }

  // Show public leaderboard for non-authenticated users
  if (!isAuthenticated) {
    return (
      <div className="min-h-screen bg-background">
        <div className="container mx-auto px-4 py-8">
          <div className="text-center mb-8">
            <div className="w-16 h-16 rounded-xl bg-primary flex items-center justify-center mx-auto mb-4">
              <Trophy className="w-8 h-8 text-primary-foreground" />
            </div>
            <h1 className="text-3xl font-bold mb-2">Leaderboard</h1>
            <p className="text-muted-foreground mb-6">
              Sign in to see the full rankings and compete with other players
            </p>
            <Button onClick={() => (window.location.href = getLoginUrl())}>
              Sign In to View
            </Button>
          </div>
        </div>
      </div>
    );
  }

  return (
    <GameLayout>
      <LeaderboardContent />
    </GameLayout>
  );
}
