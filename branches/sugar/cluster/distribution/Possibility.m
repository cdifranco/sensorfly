function p = Possibility(d, distributionTable)

p = interp1(distributionTable(:,1), distributionTable(:,4), d);