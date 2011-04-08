function rmv = remove(X, Y, Z, ord, threshold_percent, count_threshold)
%The larger the threshold_percent, the less possible the points will be removed 
count = 0;
dist_arr = zeros(1,size(X,2));
for i = 1:size(X,2)
    dist_arr(i) = ((X(i)-X(ord))^2+(Y(i)-Y(ord))^2)^.5;
end
dist_arr = sort(dist_arr);
threshold = dist_arr(round(size(X,2)*threshold_percent));
for i = 1:size(X,2)
    if (X(i) ~= X(ord) || Y(i) ~= Y(ord)) && Z(i) == Z(ord) && ((X(i)-X(ord))^2+(Y(i)-Y(ord))^2)^.5 < threshold
        count = count + 1;
    end
end

if count < count_threshold
    rmv = 1;
else 
    rmv= 0;
end