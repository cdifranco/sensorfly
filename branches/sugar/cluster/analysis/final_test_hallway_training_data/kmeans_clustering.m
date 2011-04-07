clear all;
load 'sigxyHallway.mat';
load 'parameters.mat';

%d = pdist(sigxy.sigs(:, 1:10),'euclidean');% anchor +1
%Z = linkage(d,'average');
%sigxy.cluster_id = cluster(Z,'maxclust',cluster_number,'depth',2);

[sigxy.cluster_id, sigxy.ctrs] = kmeans(sigxy.sigs(:, 1:10), cluster_number, 'Distance','sqEuclidean');% readings, center_number
colors = randperm(size(sigxy.cluster_id,1)*10);
scatter(sigxy.x,sigxy.y,75,colors(sigxy.cluster_id),'filled');

save ('kmeans_cluster.mat','sigxy');
