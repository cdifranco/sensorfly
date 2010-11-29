function p = Possibility(d,distributionTable)
%p = interp1(distributionTable(:,1), distributionTable(:,4), d);

if d < 6
    p = 1;
else
    p = 0;
end

