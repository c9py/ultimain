import GameLayout from "@/components/GameLayout";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/components/ui/select";
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow,
} from "@/components/ui/table";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import {
  Dialog,
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
} from "@/components/ui/dialog";
import { trpc } from "@/lib/trpc";
import {
  Filter,
  Loader2,
  Package,
  Search,
  ShoppingCart,
  TrendingUp,
} from "lucide-react";
import { useState } from "react";
import { useToast } from "@/hooks/use-toast";

type ListingItem = {
  listing: {
    id: number;
    companyId: number;
    businessUnitId: number;
    resourceTypeId: number;
    quantity: string;
    quality: string;
    pricePerUnit: string;
    cityId: number;
    isActive: boolean;
    createdAt: Date;
    expiresAt: Date | null;
  };
  company: { id: number; name: string } | null;
  resourceType: { id: number; name: string; icon: string | null; unit: string } | null;
  city: { id: number; name: string } | null;
};

export default function Market() {
  const { toast } = useToast();
  const [searchTerm, setSearchTerm] = useState("");
  const [selectedResource, setSelectedResource] = useState<string>("all");
  const [selectedCity, setSelectedCity] = useState<string>("all");
  const [buyDialogOpen, setBuyDialogOpen] = useState(false);
  const [selectedListing, setSelectedListing] = useState<ListingItem | null>(null);
  const [buyQuantity, setBuyQuantity] = useState(1);
  const [destinationUnit, setDestinationUnit] = useState<string>("");

  const { data: listings, isLoading, refetch: refetchListings } = trpc.market.listings.useQuery();
  const { data: resources } = trpc.resource.list.useQuery();
  const { data: cities } = trpc.city.list.useQuery();
  const { data: company } = trpc.company.mine.useQuery();
  const { data: units } = trpc.businessUnit.list.useQuery();

  const purchaseMutation = trpc.market.purchase.useMutation({
    onSuccess: () => {
      toast({
        title: "Purchase Successful",
        description: "The resources have been delivered to your unit.",
      });
      setBuyDialogOpen(false);
      setSelectedListing(null);
      setBuyQuantity(1);
      setDestinationUnit("");
      refetchListings();
    },
    onError: (error) => {
      toast({
        title: "Purchase Failed",
        description: error.message,
        variant: "destructive",
      });
    },
  });

  // Filter listings
  const filteredListings = listings?.filter((item) => {
    const matchesSearch =
      searchTerm === "" ||
      item.resourceType?.name.toLowerCase().includes(searchTerm.toLowerCase()) ||
      item.company?.name.toLowerCase().includes(searchTerm.toLowerCase());

    const matchesResource =
      selectedResource === "all" ||
      item.listing.resourceTypeId.toString() === selectedResource;

    const matchesCity =
      selectedCity === "all" ||
      item.listing.cityId.toString() === selectedCity;

    return matchesSearch && matchesResource && matchesCity;
  });

  // Group resources by category for display
  const resourcesByCategory = resources?.reduce(
    (acc, resource) => {
      if (!acc[resource.category]) {
        acc[resource.category] = [];
      }
      acc[resource.category].push(resource);
      return acc;
    },
    {} as Record<string, typeof resources>
  );

  const handleBuyClick = (item: ListingItem) => {
    setSelectedListing(item);
    setBuyQuantity(Math.min(1, parseFloat(item.listing.quantity)));
    setBuyDialogOpen(true);
  };

  const handlePurchase = () => {
    if (!selectedListing || !destinationUnit) return;
    
    purchaseMutation.mutate({
      listingId: selectedListing.listing.id,
      quantity: buyQuantity,
      destinationUnitId: parseInt(destinationUnit),
    });
  };

  const maxQuantity = selectedListing ? parseFloat(selectedListing.listing.quantity) : 0;
  const totalCost = selectedListing 
    ? buyQuantity * parseFloat(selectedListing.listing.pricePerUnit)
    : 0;
  const canAfford = company ? parseFloat(company.cash) >= totalCost : false;

  return (
    <GameLayout>
      <div className="space-y-6">
        {/* Header */}
        <div>
          <h1 className="text-3xl font-bold">Market</h1>
          <p className="text-muted-foreground">
            Buy and sell resources on the global market
          </p>
        </div>

        <Tabs defaultValue="browse" className="space-y-4">
          <TabsList>
            <TabsTrigger value="browse">
              <ShoppingCart className="w-4 h-4 mr-2" />
              Browse Listings
            </TabsTrigger>
            <TabsTrigger value="resources">
              <Package className="w-4 h-4 mr-2" />
              Resources
            </TabsTrigger>
            <TabsTrigger value="prices">
              <TrendingUp className="w-4 h-4 mr-2" />
              Price Index
            </TabsTrigger>
          </TabsList>

          <TabsContent value="browse">
            {/* Filters */}
            <Card className="mb-6">
              <CardContent className="pt-6">
                <div className="flex flex-col md:flex-row gap-4">
                  <div className="flex-1">
                    <div className="relative">
                      <Search className="absolute left-3 top-1/2 -translate-y-1/2 w-4 h-4 text-muted-foreground" />
                      <Input
                        placeholder="Search by resource or seller..."
                        value={searchTerm}
                        onChange={(e) => setSearchTerm(e.target.value)}
                        className="pl-9"
                      />
                    </div>
                  </div>
                  <Select
                    value={selectedResource}
                    onValueChange={setSelectedResource}
                  >
                    <SelectTrigger className="w-full md:w-[200px]">
                      <Filter className="w-4 h-4 mr-2" />
                      <SelectValue placeholder="Resource" />
                    </SelectTrigger>
                    <SelectContent>
                      <SelectItem value="all">All Resources</SelectItem>
                      {resources?.map((resource) => (
                        <SelectItem
                          key={resource.id}
                          value={resource.id.toString()}
                        >
                          {resource.icon} {resource.name}
                        </SelectItem>
                      ))}
                    </SelectContent>
                  </Select>
                  <Select value={selectedCity} onValueChange={setSelectedCity}>
                    <SelectTrigger className="w-full md:w-[200px]">
                      <SelectValue placeholder="City" />
                    </SelectTrigger>
                    <SelectContent>
                      <SelectItem value="all">All Cities</SelectItem>
                      {cities?.map((city) => (
                        <SelectItem key={city.id} value={city.id.toString()}>
                          {city.name}
                        </SelectItem>
                      ))}
                    </SelectContent>
                  </Select>
                </div>
              </CardContent>
            </Card>

            {/* Listings Table */}
            <Card>
              <CardHeader>
                <CardTitle>Available Listings</CardTitle>
                <CardDescription>
                  {filteredListings?.length || 0} listings found
                </CardDescription>
              </CardHeader>
              <CardContent>
                {isLoading ? (
                  <div className="flex items-center justify-center py-12">
                    <Loader2 className="w-8 h-8 animate-spin text-primary" />
                  </div>
                ) : filteredListings && filteredListings.length > 0 ? (
                  <Table>
                    <TableHeader>
                      <TableRow>
                        <TableHead>Resource</TableHead>
                        <TableHead>Seller</TableHead>
                        <TableHead>Location</TableHead>
                        <TableHead className="text-right">Quantity</TableHead>
                        <TableHead className="text-right">Quality</TableHead>
                        <TableHead className="text-right">Price/Unit</TableHead>
                        <TableHead className="text-right">Total</TableHead>
                        <TableHead></TableHead>
                      </TableRow>
                    </TableHeader>
                    <TableBody>
                      {filteredListings.map((item) => (
                        <TableRow key={item.listing.id}>
                          <TableCell>
                            <div className="flex items-center gap-2">
                              <span className="text-lg">
                                {item.resourceType?.icon || "📦"}
                              </span>
                              <span className="font-medium">
                                {item.resourceType?.name || "Unknown"}
                              </span>
                            </div>
                          </TableCell>
                          <TableCell>{item.company?.name || "Unknown"}</TableCell>
                          <TableCell>{item.city?.name || "Unknown"}</TableCell>
                          <TableCell className="text-right tabular-nums">
                            {parseFloat(item.listing.quantity).toLocaleString()}
                          </TableCell>
                          <TableCell className="text-right">
                            {(parseFloat(item.listing.quality) * 100).toFixed(0)}%
                          </TableCell>
                          <TableCell className="text-right tabular-nums">
                            ${parseFloat(item.listing.pricePerUnit).toLocaleString()}
                          </TableCell>
                          <TableCell className="text-right tabular-nums font-medium">
                            $
                            {(
                              parseFloat(item.listing.quantity) *
                              parseFloat(item.listing.pricePerUnit)
                            ).toLocaleString()}
                          </TableCell>
                          <TableCell>
                            <Button 
                              size="sm" 
                              variant="outline"
                              onClick={() => handleBuyClick(item)}
                              disabled={item.listing.companyId === company?.id}
                            >
                              {item.listing.companyId === company?.id ? "Your Listing" : "Buy"}
                            </Button>
                          </TableCell>
                        </TableRow>
                      ))}
                    </TableBody>
                  </Table>
                ) : (
                  <div className="text-center py-12">
                    <ShoppingCart className="w-12 h-12 mx-auto text-muted-foreground mb-4" />
                    <p className="text-muted-foreground">No listings found</p>
                    <p className="text-sm text-muted-foreground mt-1">
                      Try adjusting your filters or check back later
                    </p>
                  </div>
                )}
              </CardContent>
            </Card>
          </TabsContent>

          <TabsContent value="resources">
            <div className="space-y-6">
              {resourcesByCategory &&
                Object.entries(resourcesByCategory).map(([category, items]) => (
                  <Card key={category}>
                    <CardHeader>
                      <CardTitle className="capitalize">
                        {category.replace("_", " ")}
                      </CardTitle>
                    </CardHeader>
                    <CardContent>
                      <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-4">
                        {items?.map((resource) => (
                          <div
                            key={resource.id}
                            className="flex items-center gap-3 p-3 rounded-lg bg-muted/50"
                          >
                            <div className="w-10 h-10 rounded-lg bg-primary/10 flex items-center justify-center text-lg">
                              {resource.icon || "📦"}
                            </div>
                            <div className="flex-1 min-w-0">
                              <p className="font-medium truncate">
                                {resource.name}
                              </p>
                              <p className="text-sm text-muted-foreground">
                                Base: ${parseFloat(resource.basePrice).toLocaleString()} / {resource.unit}
                              </p>
                            </div>
                          </div>
                        ))}
                      </div>
                    </CardContent>
                  </Card>
                ))}
            </div>
          </TabsContent>

          <TabsContent value="prices">
            <Card>
              <CardHeader>
                <CardTitle>Market Price Index</CardTitle>
                <CardDescription>
                  Current market prices compared to base prices
                </CardDescription>
              </CardHeader>
              <CardContent>
                {resources ? (
                  <Table>
                    <TableHeader>
                      <TableRow>
                        <TableHead>Resource</TableHead>
                        <TableHead>Category</TableHead>
                        <TableHead className="text-right">Base Price</TableHead>
                        <TableHead className="text-right">Unit</TableHead>
                      </TableRow>
                    </TableHeader>
                    <TableBody>
                      {resources.map((resource) => (
                        <TableRow key={resource.id}>
                          <TableCell>
                            <div className="flex items-center gap-2">
                              <span className="text-lg">{resource.icon || "📦"}</span>
                              <span className="font-medium">{resource.name}</span>
                            </div>
                          </TableCell>
                          <TableCell className="capitalize">
                            {resource.category.replace("_", " ")}
                          </TableCell>
                          <TableCell className="text-right tabular-nums">
                            ${parseFloat(resource.basePrice).toLocaleString()}
                          </TableCell>
                          <TableCell className="text-right">
                            {resource.unit}
                          </TableCell>
                        </TableRow>
                      ))}
                    </TableBody>
                  </Table>
                ) : (
                  <div className="flex items-center justify-center py-12">
                    <Loader2 className="w-8 h-8 animate-spin text-primary" />
                  </div>
                )}
              </CardContent>
            </Card>
          </TabsContent>
        </Tabs>
      </div>

      {/* Buy Dialog */}
      <Dialog open={buyDialogOpen} onOpenChange={setBuyDialogOpen}>
        <DialogContent>
          <DialogHeader>
            <DialogTitle>Purchase Resources</DialogTitle>
            <DialogDescription>
              Buy {selectedListing?.resourceType?.name} from {selectedListing?.company?.name}
            </DialogDescription>
          </DialogHeader>
          
          <div className="space-y-4 py-4">
            <div className="p-4 rounded-lg bg-muted">
              <div className="flex items-center gap-3 mb-3">
                <span className="text-2xl">{selectedListing?.resourceType?.icon || "📦"}</span>
                <div>
                  <p className="font-semibold">{selectedListing?.resourceType?.name}</p>
                  <p className="text-sm text-muted-foreground">
                    ${parseFloat(selectedListing?.listing.pricePerUnit || "0").toLocaleString()} per unit
                  </p>
                </div>
              </div>
              <div className="text-sm text-muted-foreground">
                Available: {maxQuantity.toLocaleString()} units
              </div>
            </div>

            <div className="space-y-2">
              <Label>Quantity to Purchase</Label>
              <Input
                type="number"
                min={1}
                max={maxQuantity}
                value={buyQuantity}
                onChange={(e) => setBuyQuantity(Math.min(maxQuantity, Math.max(1, parseInt(e.target.value) || 1)))}
              />
            </div>

            <div className="space-y-2">
              <Label>Destination Unit</Label>
              <Select value={destinationUnit} onValueChange={setDestinationUnit}>
                <SelectTrigger>
                  <SelectValue placeholder="Select a unit to receive goods" />
                </SelectTrigger>
                <SelectContent>
                  {units?.map((unit) => (
                    <SelectItem key={unit.id} value={unit.id.toString()}>
                      {unit.name} ({unit.type})
                    </SelectItem>
                  ))}
                </SelectContent>
              </Select>
            </div>

            <div className="p-4 rounded-lg border">
              <div className="flex justify-between mb-2">
                <span>Quantity:</span>
                <span>{buyQuantity.toLocaleString()} units</span>
              </div>
              <div className="flex justify-between mb-2">
                <span>Price per unit:</span>
                <span>${parseFloat(selectedListing?.listing.pricePerUnit || "0").toLocaleString()}</span>
              </div>
              <div className="flex justify-between font-semibold text-lg border-t pt-2">
                <span>Total Cost:</span>
                <span className={canAfford ? "text-green-500" : "text-red-500"}>
                  ${totalCost.toLocaleString()}
                </span>
              </div>
              {!canAfford && (
                <p className="text-sm text-red-500 mt-2">
                  Insufficient funds. You have ${parseFloat(company?.cash || "0").toLocaleString()}
                </p>
              )}
            </div>
          </div>

          <DialogFooter>
            <Button variant="outline" onClick={() => setBuyDialogOpen(false)}>
              Cancel
            </Button>
            <Button 
              onClick={handlePurchase}
              disabled={!canAfford || !destinationUnit || purchaseMutation.isPending}
            >
              {purchaseMutation.isPending ? (
                <>
                  <Loader2 className="w-4 h-4 mr-2 animate-spin" />
                  Processing...
                </>
              ) : (
                "Confirm Purchase"
              )}
            </Button>
          </DialogFooter>
        </DialogContent>
      </Dialog>
    </GameLayout>
  );
}
