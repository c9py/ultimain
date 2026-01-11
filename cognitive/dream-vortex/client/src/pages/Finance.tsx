import GameLayout from "@/components/GameLayout";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow,
} from "@/components/ui/table";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { trpc } from "@/lib/trpc";
import {
  ArrowDownRight,
  ArrowUpRight,
  DollarSign,
  Loader2,
  TrendingDown,
  TrendingUp,
  Wallet,
} from "lucide-react";

export default function Finance() {
  const { data: company } = trpc.company.mine.useQuery();
  const { data: transactions, isLoading } = trpc.transaction.list.useQuery({
    limit: 100,
  });

  // Calculate summary stats
  const income =
    transactions
      ?.filter((t) => parseFloat(t.amount) > 0)
      .reduce((sum, t) => sum + parseFloat(t.amount), 0) || 0;

  const expenses =
    transactions
      ?.filter((t) => parseFloat(t.amount) < 0)
      .reduce((sum, t) => sum + Math.abs(parseFloat(t.amount)), 0) || 0;

  const netProfit = income - expenses;

  // Group transactions by type
  const transactionsByType = transactions?.reduce(
    (acc, tx) => {
      if (!acc[tx.type]) {
        acc[tx.type] = { count: 0, total: 0 };
      }
      acc[tx.type].count++;
      acc[tx.type].total += parseFloat(tx.amount);
      return acc;
    },
    {} as Record<string, { count: number; total: number }>
  );

  const getTransactionIcon = (type: string) => {
    switch (type) {
      case "sale":
        return <ArrowUpRight className="w-4 h-4 text-success" />;
      case "purchase":
        return <ArrowDownRight className="w-4 h-4 text-destructive" />;
      case "salary":
        return <DollarSign className="w-4 h-4 text-orange-500" />;
      case "tax":
        return <DollarSign className="w-4 h-4 text-red-500" />;
      case "construction":
        return <ArrowDownRight className="w-4 h-4 text-blue-500" />;
      default:
        return <DollarSign className="w-4 h-4 text-muted-foreground" />;
    }
  };

  return (
    <GameLayout>
      <div className="space-y-6">
        {/* Header */}
        <div>
          <h1 className="text-3xl font-bold">Finance</h1>
          <p className="text-muted-foreground">
            Track your company's financial performance
          </p>
        </div>

        {/* Summary Cards */}
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
          <Card>
            <CardContent className="pt-6">
              <div className="flex items-center justify-between">
                <div>
                  <p className="text-sm text-muted-foreground">Cash Balance</p>
                  <p className="text-2xl font-bold tabular-nums">
                    ${company ? parseFloat(company.cash).toLocaleString() : "0"}
                  </p>
                </div>
                <div className="w-12 h-12 rounded-full bg-primary/10 flex items-center justify-center">
                  <Wallet className="w-6 h-6 text-primary" />
                </div>
              </div>
            </CardContent>
          </Card>

          <Card>
            <CardContent className="pt-6">
              <div className="flex items-center justify-between">
                <div>
                  <p className="text-sm text-muted-foreground">Total Income</p>
                  <p className="text-2xl font-bold tabular-nums text-success">
                    +${income.toLocaleString()}
                  </p>
                </div>
                <div className="w-12 h-12 rounded-full bg-success/10 flex items-center justify-center">
                  <TrendingUp className="w-6 h-6 text-success" />
                </div>
              </div>
            </CardContent>
          </Card>

          <Card>
            <CardContent className="pt-6">
              <div className="flex items-center justify-between">
                <div>
                  <p className="text-sm text-muted-foreground">Total Expenses</p>
                  <p className="text-2xl font-bold tabular-nums text-destructive">
                    -${expenses.toLocaleString()}
                  </p>
                </div>
                <div className="w-12 h-12 rounded-full bg-destructive/10 flex items-center justify-center">
                  <TrendingDown className="w-6 h-6 text-destructive" />
                </div>
              </div>
            </CardContent>
          </Card>

          <Card>
            <CardContent className="pt-6">
              <div className="flex items-center justify-between">
                <div>
                  <p className="text-sm text-muted-foreground">Net Profit</p>
                  <p
                    className={`text-2xl font-bold tabular-nums ${
                      netProfit >= 0 ? "text-success" : "text-destructive"
                    }`}
                  >
                    {netProfit >= 0 ? "+" : ""}${netProfit.toLocaleString()}
                  </p>
                </div>
                <div
                  className={`w-12 h-12 rounded-full flex items-center justify-center ${
                    netProfit >= 0 ? "bg-success/10" : "bg-destructive/10"
                  }`}
                >
                  <DollarSign
                    className={`w-6 h-6 ${
                      netProfit >= 0 ? "text-success" : "text-destructive"
                    }`}
                  />
                </div>
              </div>
            </CardContent>
          </Card>
        </div>

        <Tabs defaultValue="transactions" className="space-y-4">
          <TabsList>
            <TabsTrigger value="transactions">Transactions</TabsTrigger>
            <TabsTrigger value="breakdown">Breakdown</TabsTrigger>
          </TabsList>

          <TabsContent value="transactions">
            <Card>
              <CardHeader>
                <CardTitle>Transaction History</CardTitle>
                <CardDescription>
                  Recent financial transactions for your company
                </CardDescription>
              </CardHeader>
              <CardContent>
                {isLoading ? (
                  <div className="flex items-center justify-center py-12">
                    <Loader2 className="w-8 h-8 animate-spin text-primary" />
                  </div>
                ) : transactions && transactions.length > 0 ? (
                  <Table>
                    <TableHeader>
                      <TableRow>
                        <TableHead>Type</TableHead>
                        <TableHead>Description</TableHead>
                        <TableHead>Date</TableHead>
                        <TableHead className="text-right">Amount</TableHead>
                      </TableRow>
                    </TableHeader>
                    <TableBody>
                      {transactions.map((tx) => (
                        <TableRow key={tx.id}>
                          <TableCell>
                            <div className="flex items-center gap-2">
                              {getTransactionIcon(tx.type)}
                              <span className="capitalize">{tx.type}</span>
                            </div>
                          </TableCell>
                          <TableCell className="max-w-xs truncate">
                            {tx.description || "-"}
                          </TableCell>
                          <TableCell>
                            {new Date(tx.createdAt).toLocaleDateString()}
                          </TableCell>
                          <TableCell
                            className={`text-right tabular-nums font-medium ${
                              parseFloat(tx.amount) >= 0
                                ? "text-success"
                                : "text-destructive"
                            }`}
                          >
                            {parseFloat(tx.amount) >= 0 ? "+" : ""}$
                            {Math.abs(parseFloat(tx.amount)).toLocaleString()}
                          </TableCell>
                        </TableRow>
                      ))}
                    </TableBody>
                  </Table>
                ) : (
                  <div className="text-center py-12">
                    <DollarSign className="w-12 h-12 mx-auto text-muted-foreground mb-4" />
                    <p className="text-muted-foreground">No transactions yet</p>
                    <p className="text-sm text-muted-foreground mt-1">
                      Your financial history will appear here
                    </p>
                  </div>
                )}
              </CardContent>
            </Card>
          </TabsContent>

          <TabsContent value="breakdown">
            <Card>
              <CardHeader>
                <CardTitle>Financial Breakdown</CardTitle>
                <CardDescription>
                  Summary of transactions by category
                </CardDescription>
              </CardHeader>
              <CardContent>
                {transactionsByType &&
                Object.keys(transactionsByType).length > 0 ? (
                  <div className="space-y-4">
                    {Object.entries(transactionsByType).map(([type, data]) => (
                      <div
                        key={type}
                        className="flex items-center justify-between p-4 rounded-lg bg-muted/50"
                      >
                        <div className="flex items-center gap-3">
                          {getTransactionIcon(type)}
                          <div>
                            <p className="font-medium capitalize">{type}</p>
                            <p className="text-sm text-muted-foreground">
                              {data.count} transaction{data.count !== 1 ? "s" : ""}
                            </p>
                          </div>
                        </div>
                        <p
                          className={`text-lg font-bold tabular-nums ${
                            data.total >= 0 ? "text-success" : "text-destructive"
                          }`}
                        >
                          {data.total >= 0 ? "+" : ""}$
                          {Math.abs(data.total).toLocaleString()}
                        </p>
                      </div>
                    ))}
                  </div>
                ) : (
                  <div className="text-center py-12">
                    <DollarSign className="w-12 h-12 mx-auto text-muted-foreground mb-4" />
                    <p className="text-muted-foreground">No data available</p>
                  </div>
                )}
              </CardContent>
            </Card>
          </TabsContent>
        </Tabs>
      </div>
    </GameLayout>
  );
}
