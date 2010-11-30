basic_time = [];
basic_accuracy = [];
kmeans_time = [];
kmeans_accuracy = [];

for i = 1:size(time,2)
    if mod(i,2)==1 
        basic_time = [basic_time time(i)];
        basic_accuracy = [basic_accuracy accuracy(i)];
    else 
        kmeans_time = [kmeans_time time(i)];
        kmeans_accuracy = [kmeans_accuracy accuracy(i)];
    end
end

plot(10:9+size(time,2)/2,basic_time,'r',10:9+size(time,2)/2,kmeans_time,'b');
figure;
plot(10:9+size(time,2)/2,basic_accuracy,'r',10:9+size(time,2)/2,kmeans_accuracy,'b');