library(ggplot2)
library(grid)
library(scales)
library(stringr)
library(plyr)
library(dplyr)
library(reshape2)

theme_complete_bw <- function(base_size = 12, base_family = "") {
  theme(
    line =               element_line(colour = "black", size = 0.5, linetype = 1,
                            lineend = "butt"),
    rect =               element_rect(fill = "white", colour = "black", size = 0.5, linetype = 1),
    text =               element_text(family = base_family, face = "plain",
                            colour = "black", size = base_size,
                            hjust = 0.5, vjust = 0.5, angle = 0, lineheight = 0.9,
                            margin = margin(), debug = FALSE),
    axis.text =          element_text(size = rel(0.8), colour = "grey50"),
    strip.text =         element_text(size = base_size * 0.7),
    axis.line =          element_blank(),
    axis.text.x =        element_blank(),
    axis.text.y =        element_text(size = base_size * 0.75, lineheight = 0.9, 
                                      colour = "black", hjust = 1, margin=margin(3,3,3,3,unit="mm")),
    axis.ticks =         element_line(colour = "black"),
    axis.title.x =       element_blank(),
    axis.title.y =       element_text(size = base_size * 1, angle = 90, vjust = 0.5),
    axis.ticks.length =  unit(0.15, "cm"),
    #axis.ticks.margin =  unit(0.1, "cm"),

    legend.background =  element_blank(),
    legend.margin =      unit(0.1, "cm"),
    legend.key.height =  unit(1, "cm"),
    legend.key.width =   unit(1, "cm"),
    legend.text =        element_text(size = rel(1.25)),
    legend.text.align =  NULL,
    legend.title =       element_blank(),
    legend.title.align = NULL,
    legend.direction =   "horizontal",
    legend.justification = "center",
    legend.box =         NULL,
    legend.position =   "bottom",

    plot.background =    element_rect(colour = NA, fill = "white"),
    plot.title =         element_text(size = base_size * 1.5, margin = margin(5,5,5,5, unit="mm")),
    plot.margin=         unit(c(5,5,5,5),"mm"),
    complete = TRUE
  )
}

artificial <- c("fib41","rs.13","tm29")
pseudo_real <- c("dblp.xml.00001.1","dblp.xml.00001.2","dblp.xml.0001.1","dblp.xml.0001.2",
                 "dna.001.1","english.001.2","proteins.001.1","sources.001.2")
real <- c("Escherichia_Coli","cere","coreutils","einstein.de.txt","einstein.en.txt",
          "influenza","kernel","para","world_leaders")

types <- c("artificial", "pseudo_real", "real")

benchmark_type <- function(row) {
  if(row['Benchmark'] %in% real) {
    return("real")
  }
  else if(row["Benchmark"] %in% pseudo_real) {
    return("pseudo-real")
  }
  else if(row["Benchmark"] %in% artificial) {
    return("artificial")
  }
}

create_mean_data_frame_for_types <- function(vec, colname) {
  df <- data.frame(Vector=vec$Vector,Type=vec$Type,x=colname)
  df$x <- as.numeric(as.character(df$x))
  df <- aggregate(df$x, by=list(Vector = df$Vector, Type = df$Type), mean, drop=TRUE)
  return(df)
}

vector_plot_for_benchmark <- function(vec, colname, yaxis="", title="") {
  
  plot <- ggplot(data=vec,aes(x=Vector,y=colname,fill=Vector,label=round(colname,digits=2))) + geom_bar(stat="identity")
  plot <- plot + facet_wrap(~ Benchmark, scales="free") + geom_text(vjust=-0.075, check_overlap=TRUE)
  plot <- plot + ylab(yaxis)
  #plot <- plot + xlab("Range")
  plot <- plot + ggtitle(title) + theme_complete_bw()
  print(plot)
}

vector_plot_for_types <- function(vec, colname, yaxis="", title="") {
  df <- create_mean_data_frame_for_types(vec, colname)
  df <- df[order(df$Type),]
  
  plot <- ggplot(data=df,aes(x=Vector,y=x,fill=Vector,label=round(x,digits=2))) + geom_bar(stat="identity")
  plot <- plot + facet_wrap(~ Type) + geom_text(vjust=-0.075, check_overlap=TRUE)
  plot <- plot + ylab(yaxis)
  #plot <- plot + xlab("Range")
  plot <- plot + ggtitle(title) + theme_complete_bw()
  print(plot)
}

#==========Experiment===========#
setwd("C:/Users/tobia/Documents/home/theuer/rlcsa-experiments/results/")
experiment_dir="C:/Users/tobia/Documents/home/theuer/rmq-experiments/results/"
date="2017-04-26"
experiment_number="0"
tmp <- cbind(date,"rl_vector_experiment",experiment_number)
experiment <- str_c(tmp,collapse='_');
#experiment <- paste(experiment_dir,experiment,sep="")

rlvec <- read.csv2(paste(experiment,"/rlvector_result.csv",sep=""),sep=",",header=TRUE)
rlvec$Type = as.factor(apply(rlvec,1,function(x) benchmark_type(x)))
rlvec$SpaceBitsPerElement <- as.numeric(as.character(rlvec$SpaceBitsPerElement))
rlvec$RandomAccessTimePerElement <- as.numeric(as.character(rlvec$RandomAccessTimePerElement))
rlvec$SequentialAccessTimePerElement <- as.numeric(as.character(rlvec$SequentialAccessTimePerElement))
rlvec <- rlvec[order(rlvec$Benchmark),]

vector_plot_for_benchmark(rlvec,rlvec$SpaceBitsPerElement, yaxis="Space in bits per element", title="Space in bits per element for all benchmarks")
vector_plot_for_benchmark(rlvec,rlvec$RandomAccessTimePerElement, yaxis="Random access time per element [탎]", title="Random access time per element for all benchmarks")
vector_plot_for_benchmark(rlvec,rlvec$SequentialAccessTimePerElement, yaxis="Sequential access time per element [탎]", title="Sequential access time per element for all benchmarks")


vector_plot_for_types(rlvec,rlvec$SpaceBitsPerElement, yaxis="Space in bits per element", title="Space in bits per element for benchmark types")
vector_plot_for_types(rlvec,rlvec$RandomAccessTimePerElement, yaxis="Random access time per element [탎]", title="Random access time per element for benchmark types")
vector_plot_for_types(rlvec,rlvec$SequentialAccessTimePerElement, yaxis="Sequential access time per element [탎]", title="Sequential access time per element for benchmark types")

