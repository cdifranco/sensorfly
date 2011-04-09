load '../2_del_comma/del_comma_data/matlab_var.mat'
sig_dist = cell(72,2);
for i = 10:10:720
   eval(['sig_dist(' int2str(i/10) ', 1:2)={' int2str(i) ',sig_dist_' int2str(i) 'cm(:,4)''};']);  
   eval(['sig_dist(' int2str(i/10) ', 3)={mean(sig_dist{' int2str(i/10) ', 2})};']);
end
clear i;