function a = draw_cluster(b,center_size,reading)
% b is the position of clustering record
colors = randperm(center_size*2);
scatter(reading(:,3),reading(:,4),30,colors(reading(:,b)),'filled');
