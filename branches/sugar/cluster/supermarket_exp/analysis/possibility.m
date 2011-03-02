function p = possibility(d,distributionTable)
d_in_meter = 0.3048 * d;
p = interp1(distributionTable(:,1), distributionTable(:,4), d_in_meter);