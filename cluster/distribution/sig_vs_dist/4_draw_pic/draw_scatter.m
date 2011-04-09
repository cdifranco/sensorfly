points = zeros(size(sig_dist,1)*100,2)
for i = 1:size(sig_dist,1)
    for j = 1:100
    points((i-1)*100+j,:) = [sig_dist{i,1}/100 sig_dist{i,2}(j)];
    end
end

scatter(points(:,1), points(:,2), 5, zeros(size(sig_dist,1)*100, 1), 'filled','b');
xlabel('Real Distance (m)');
ylabel('Ranging Distance');
%axis equal;
%axis tight;