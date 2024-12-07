from PIL import Image

def convert_tga_to_rgb(input_path, output_path):
    # 打开灰度 TGA 图像
    gray_image = Image.open(input_path).convert("L")  # 确保是灰度模式
    
    # 将灰度图像扩展为 RGB
    rgb_image = gray_image.convert("RGB")
    
    # 保存为 RGB 图像
    rgb_image.save(output_path)
    print(f"转换完成，保存至: {output_path}")

# 示例使用
input_tga = "afraican_head_spec.tga"  # 替换为你的灰度 TGA 文件路径
output_tga = "output_rgb.tga"  # 替换为输出 RGB TGA 文件路径
convert_tga_to_rgb(input_tga, output_tga)
