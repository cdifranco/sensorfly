 plot([sig_dist{:,1}]./100,[sig_dist{:,3}]);hold on;
 lim = min(max([sig_dist{:,1}]./100), max([sig_dist{:,3}]));
 plot(linspace(0,lim,100),linspace(0,lim,100),'r');
 axis equal;
 axis tight;