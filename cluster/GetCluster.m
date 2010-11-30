function cluster = GetCluster(centers, sig)

dist = zeros(1, size(centers,1));
for i = 1:size(centers,1)
    center_sig = centers(i,4:end);
    dist(i) = sum((sig - center_sig).^2).^.5;
end

[minDist cluster] = min(dist);

clear minDist;