function DrawDistribution(distriTable, low, high)

X = low:0.01:high;
Y = interp1(distriTable(:, 1), distriTable(:, 4), X);
plot(X, Y);