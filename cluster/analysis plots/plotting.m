basic_time = [];
basic_accuracy = [];
kmeans_time = [];
kmeans_accuracy = [];

for i = 1:size(time,1)
    if mod(i,2)==1 
        basic_time = [basic_time time(i)];
        basic_accuracy = [basic_accuracy accuracy(i)];
    else 
        kmeans_time = [kmeans_time time(i)];
        kmeans_accuracy = [kmeans_accuracy accuracy(i)];
    end
end

plot(4:8,basic_time,'r',4:8,kmeans_time,'b');
figure;
plot(4:8,basic_accuracy,'r',4:8,kmeans_accuracy,'b');