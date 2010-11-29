function rangeDist = get_reading_fit_noise(real_dist, n, noise_rate, sig_mean_dist)

coefficient = polyfit(sig_mean_dist(:, 1), sig_mean_dist(: ,2), n);
random = rand(1,length(real_dist));
rangeDist = polyval(coefficient, real_dist').*(1 + (2*random-1)*noise_rate);
h = 1;
