clear
close all;
src_image_name = 'goldhill.bmp';

crs_image_name = 'temp.out';
dst_image_name = sprintf('%s_restored%s', src_image_name(1 : end - 4), src_image_name(end - 3 : end));
src_image_name = sprintf('%s', src_image_name);
src_image = double(imread(src_image_name));
src_image = src_image(:, :, 1);

%lambda_arr = [150 190 250];
%lambda_arr = [4:2:21, 26 29 40 50 70 100];
a = imread(src_image_name);
a = double(a(:,:,1));

% q_opt_arr = [];
% options = optimset('TolX', 0.005);
% for lambda = lambda_arr
%   [q_opt, ymin] = fminbnd(@(q) get_J(q, lambda, src_image_name,dst_image_name,crs_image_name, a), 25, 60, options);
%   q_opt_arr = [q_opt_arr, q_opt];
% end
% return;

% barbara
% lambda_arr = [4:2:21, 26 29 40 50 70 100 150 190 250 350 400];
% q_opt_arr = [5.4959    6.9815    8.2734    8.3265   10.1745   10.0919...
%              11.6194   12.0087   12.8409   14.7793   15.2168   17.0776...
%              21.0141   23.5603   28.2544   33.3245   39.9976   44.2601...
%              51.7490   54.0923];

% Lena
% lambda_arr = [4:2:21, 26 29 40 50 70 100];
%% lambda=0:21:::
% q_opt_arr = [1.0000    2.7333    4.0459    4.9919...
%              5.5228    5.8260    7.3115    7.3246    8.0616...
%              8.8091    9.1955    9.8077   10.0151   10.0109   11.1724...
%              11.1566   11.2612   12.0047   12.8875   12.8409   12.9159 13.3852];
% q_opt_arr = [q_opt_arr(5:2:end), [14.3257 15.8672 18.192 21.6363 24.4149 29.413]]; 

% Goldhill
lambda_arr = [4:2:21, 26 29 40 50 70 100 150 190 250];
q_opt_arr = [6.0482    6.8807    8.3069    8.9009   10.0588   10.7279...
             11.3839   13.0160   12.7719   14.9192   15.6889   18.7858...
             20.2796   24.8896   28.0664   34.4421   37.7732   47.0724];

result = [];
psnr = [];
bpp = [];
figure;
for i = 1:length(lambda_arr)
    lambda = lambda_arr(i);
    q = q_opt_arr(i);
    
    cmd = sprintf('.\\cwrap.exe %s %s %f %f', src_image_name, dst_image_name, q, lambda)
    system(cmd);
    dst_image = double(imread(dst_image_name));
    dst_image = dst_image(:, :, 1);

    psnr = [psnr 10 * log10(255^2 * numel(src_image) / sum(sum((dst_image - src_image).^2)))];
    s =  dir(crs_image_name);
    bpp = [bpp s.bytes * 8 / numel(src_image)];

    result = [result struct('q', q, 'lambda', lambda, 'psnr', psnr(end), 'bpp', bpp(end))];
    delete(dst_image_name);
    delete(crs_image_name);
    
end


plot(bpp, psnr, '-ks', 'MarkerFaceColor', 'k', 'MarkerSize', 4), hold on, grid on

for i = 1:length(lambda_arr)
    text(bpp(i) + .015, psnr(i)-0.05, sprintf('q=%.1f \\lambda=%.1f', q_opt_arr(i), lambda_arr(i)), 'HorizontalAlignment','left','BackgroundColor', 'white')
end
plot(bpp, psnr, '-ks', 'MarkerFaceColor', 'k', 'MarkerSize', 4), hold on, grid on

title(sprintf('%s; lambda = [%s ]', src_image_name(1:end-4), num2str(lambda_arr)));
xlabel('bpp');
ylabel('psnr');

[[result.q]' [result.lambda]' [result.psnr]' [result.bpp]']

