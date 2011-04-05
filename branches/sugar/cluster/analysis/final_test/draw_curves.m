plot(5:5:5+(size(dist_error_collection_contolled, 1)-1)*5, dist_error_collection_contolled(:,1),'b','-');
errorbar(5:5:5+(size(dist_error_collection_contolled, 1)-1)*5, dist_error_collection_contolled(:,1), dist_error_collection_contolled(:,2));
hold on;
plot(5:5:5+(size(dist_error_collection_random, 1)-1)*5, dist_error_collection_random(:,1),'b','-');
errorbar(5:5:5+(size(dist_error_collection_random, 1)-1)*5, dist_error_collection_random(:,1), dist_error_collection_random(:,2));
hold off;
