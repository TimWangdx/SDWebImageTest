//
//  ViewController.m
//  SDWebImageTest
//
//  Created by HaKim on 16/7/5.
//  Copyright © 2016年 haKim. All rights reserved.
//

#import "ViewController.h"
#import "UIImageView+WebCache.h"

@interface ViewController ()
@property (weak, nonatomic) IBOutlet UIImageView *imageView;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    NSURL *url = [NSURL URLWithString:@"https://oss.duc365.com/file/20160620/201606201621272022042737.jpg"];
    [self.imageView setShowActivityIndicatorView:YES];
    [self.imageView setIndicatorStyle:UIActivityIndicatorViewStyleGray];
//    [self.imageView sd_setImageWithURL:url completed:^(UIImage *image, NSError *error, SDImageCacheType cacheType, NSURL *imageURL) {
//        
//    }];
    [self.imageView sd_setImageWithURL:url placeholderImage:nil options:SDWebImageProgressiveDownload completed:^(UIImage *image, NSError *error, SDImageCacheType cacheType, NSURL *imageURL) {
        
    }];
    
    SDImageCache *cache = [SDImageCache sharedImageCache];
    
    SDWebImageManager *manager = [SDWebImageManager sharedManager];
    
    NSString *key = [manager cacheKeyForURL:url];
    
    NSString *path = [cache defaultCachePathForKey:key];
    
    NSLog(@"path = %@",path);
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
