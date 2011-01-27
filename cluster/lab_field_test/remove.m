function rmv = remove(X, Y, Z, ord, threshold1, threshold2)

far = 1;
alone = 1;
for i = 1:size(X)
    if i ~= ord && Z(i) == Z(ord) && ((X(i)-X(ord))^2+(Y(i)-Y(ord))^2)^.5 < threshold1
        far = 0;
        break;
    elseif i ~= ord && Z(i) ~= Z(ord) && ((X(i)-X(ord))^2+(Y(i)-Y(ord))^2)^.5 < threshold2
        alone = 0;
    end
end

if far == 1 && alone == 0
    rmv = 1;
else
    rmv = 0;
end