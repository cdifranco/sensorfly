clear all;
load 'sigxyLab.mat';
load 'parameters.mat';

opts = statset('Display','final');
[sigxy.cluster_id, sigxy.ctrs] = kmeans(sigxy.sigs(:, 1:10), cluster_number, 'Distance','city', 'Replicates',20, 'Options',opts);% readings, center_number
colors = randperm(size(sigxy.ctrs,1));
scatter(sigxy.x,sigxy.y,75,colors(sigxy.cluster_id),'filled');

if cluster_number == 10
    save ('kmeans_cluster.mat','sigxy');
end
