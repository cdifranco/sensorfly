colors = randperm(size(center,1));
scatter(reading(:,2),reading(:,3),75,colors(reading(:,1)),'filled');