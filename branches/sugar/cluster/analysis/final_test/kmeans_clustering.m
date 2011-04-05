clear all;
load 'sigxyLab.mat';
cluster_number = 10;

opts = statset('Display','final');
[sigxy.cluster_id, sigxy.ctrs] = kmeans(sigxy.sigs(:, 1:10), 10, 'Distance','city', 'Replicates',20, 'Options',opts);% readings, center_number
colors = randperm(size(sigxy.ctrs,1));
scatter(sigxy.x,sigxy.y,75,colors(sigxy.cluster_id),'filled');

%{
trans_history_new = ones(size(center_new,1),direction_number,size(center_new,1)) * trans_init_number;%update the trans_history
for mainloop = 1 : size(reading,2)
    if mainloop ~= 1
        trans_history_new(reading(mainloop-1,end), reading(mainloop-1,4), reading(mainloop,end)) = trans_history_new(reading(mainloop-1,end), reading(mainloop-1,4), reading(mainloop,end)) + 1;
    end
end
%}

save 'kmeans_cluster.mat';
