function rangeDist = get_reading_fit_noise(real_dist, noise_rate,coefficient)
random = rand(1,length(real_dist));
rangeDist = polyval(coefficient, real_dist').*(1 + (2*random-1)*noise_rate);
