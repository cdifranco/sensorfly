function r = draw_single_cluster(cluster_id, center, reading)
colors = randperm(size(center,1));
reading_subset = reading(reading(:, 1) == cluster_id, :);
scatter(reading_subset(:,2),reading_subset(:,3),75,colors(reading_subset(:,1)),'filled');