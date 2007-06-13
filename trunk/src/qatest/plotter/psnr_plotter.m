function psnr_plotter(src_image_name)

clc

dst_image_name = sprintf('%s_restored%s', src_image_name(1 : end - 4), src_image_name(end - 3 : end));
src_image = double(imread(src_image_name));

result = [];
for q = 1 : 1 : 20
    psnr = [];
    bpp = [];
    for lambda = 1 : 1 : 50
        system(sprintf('codec.exe %s %s %f %f', src_image_name, dst_image_name, q, lambda));
        dst_image = double(imread(dst_image_name));

        psnr = [psnr 10 * log10(255^2 * numel(src_image) / sum(sum((dst_image - src_image).^2)))];
        dst_image_info =  imfinfo(dst_image_name);
        bpp = [bpp dst_image_info.FileSize * 8 / numel(src_image)];

        result = [result struct('q', q, 'lambda', lambda, 'psnr', psnr, 'bpp', bpp)];
        delete(dst_image_name)
    end
    plot(bpp, psnr, '-ks', 'MarkerFaceColor', 'k', 'MarkerSize', 4), hold on, grid on
    text(bpp(end), psnr(end), sprintf('  q = %d', q), 'HorizontalAlignment','left',  'BackgroundColor', 'white')
end

title(sprintf('%s; lambda = [ %s ]', src_image_name, num2str(unique([result.lambda]))));
xlabel('bpp');
ylabel('psnr');

[[result.q]' [result.lambda]' [result.psnr]' [result.bpp]']