function restore()
    angle_num = 57;
    pic_per_angle = 5;
    for i = 1:angle_num
        final_pic = single_restore("data/" + num2str(i) + "/", pic_per_angle);
        edge_pic = edge(rgb2gray(final_pic), "sobel");
        edge_pic = imgaussfilt(double(edge_pic), 2);
        index = edge_pic > 0.3;

        filter_pic = zeros(size(final_pic));
        for j = 1:3
            pic = final_pic(:,:,j);
            pic(index) = 0;
            pic2 = medfilt2(pic, [5, 3]);
            pic(pic2 == 0) = 0;

            filter_pic(:,:,j) = pic;
        end
        imwrite(filter_pic, "result/" + num2str(i) + ".png");
    end
end

function final_pic = single_restore(directory, pic_per_angle)
    img = imread(directory + "1.jpg");
    init_size = size(img);
    img = rgb2gray(img);

    pixel_num = size(img, 1) * size(img, 2);
    img = reshape(img, [1, pixel_num]);
    final_pic = zeros([pixel_num, 3]);

    index = 1:pixel_num;
    index = index(img ~= 0);

    I = zeros(size(index, 2), pic_per_angle);
    I(:, 1) = img(index);

    for i = 2:pic_per_angle
        img = rgb2gray(imread(directory + num2str(i) + ".jpg"));
        img = reshape(img, [1, pixel_num]);
        I(:, i) = img(index);
    end

    file = fopen(directory + "light_dir.txt");
    light_dir = fscanf(file, "%f", [3, pic_per_angle]);
    fclose(file);
    
    N = I * pinv(light_dir);
    N(isnan(N)) = 0;
    len_mat = sqrt(sum(N .^ 2, 2));
    N = N ./ len_mat;
    N(isnan(N)) = 0;

    file = fopen(directory + "camera_dir.txt");
    camera_mat = fscanf(file, "%f", [3, 3]);
    N = N * camera_mat;
    fclose(file);

    color_N = (N + 3) / 4;

    final_pic(index, :) = color_N;
    final_pic = reshape(final_pic, init_size);
end
