function rangeDist = get_reading_fit(real_dist, n, sig_mean_dist)

coefficient = polyfit(sig_mean_dist(:, 1), sig_mean_dist(: ,2), n);
rangeDist = polyval(coefficient, real_dist);