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
    axis.title.x =       element_text(size = base_size * 1, vjust = 0.5),
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

aggreg <- function(df) data.frame(ConstructionTime=mean(df$ConstructionTime),
                                  SpaceBitsPerElement=mean(df$SpaceBitsPerElement),
                                  RandomAccessTimePerElement=mean(df$RandomAccessTimePerElement),
                                  SequentialAccessTimePerElement=mean(df$SequentialAccessTimePerElement))


vector_space_plot <- function(vec, yaxis="", title="Space in bits per element") {
  plot <- ggplot(data=vec,aes(x=Vector,y=SpaceBitsPerElement,fill=Vector,label=round(SpaceBitsPerElement,digits=2))) + geom_bar(stat="identity")
  plot <- plot + geom_text(vjust=-0.075, check_overlap=TRUE)
  plot <- plot + ylab(yaxis)
  #plot <- plot + xlab("Range")
  plot <- plot + ggtitle(title) + theme_complete_bw()
  return(plot)
}

vector_access_time_plot <- function(vec, yaxis="", title="Random access time per element [µs]") {
  plot <- ggplot(data=vec,aes(x=Vector,y=RandomAccessTimePerElement,fill=Vector,label=round(RandomAccessTimePerElement,digits=2))) + geom_bar(stat="identity")
  plot <- plot + geom_text(vjust=-0.075, check_overlap=TRUE)
  plot <- plot + ylab(yaxis)
  #plot <- plot + xlab("Range")
  plot <- plot + ggtitle(title) + theme_complete_bw()
  return(plot)
}


time_space_tradeoff_plot_benchmark <- function(vec, title="Access Time-Space-Tradeoff", point_size=3) {
  df <- subset(vec, vec$Vector != "enc_vector")
  
  plot <- ggplot(data=df,aes(x=RandomAccessTimePerElement,y=SpaceBitsPerElement,colour=factor(Vector))) 
  plot <- plot + geom_point(size=point_size)
  plot <- plot + xlab("Random access time per element [µs]")
  plot <- plot + ylab("Space in bits per element")
  plot <- plot + ggtitle(title) + theme_complete_bw()
  plot <- plot + theme(axis.text.x = element_text(size = 11 * 0.75, lineheight = 0.9, 
                                                  colour = "black", hjust = 1, margin=margin(3,3,3,3,unit="mm")))
  return(plot)
}

#==========Experiment===========#
setwd("C:/Users/tobia/Documents/home/theuer/rlcsa-experiments/results/")
experiment_dir="C:/Users/tobia/Documents/home/theuer/rmq-experiments/results/"
date="2017-04-26"
experiment_number="1"
tmp <- cbind(date,"rl_vector_experiment",experiment_number)
experiment <- str_c(tmp,collapse='_');
#experiment <- paste(experiment_dir,experiment,sep="")

rlvec <- read.csv2(paste(experiment,"/rlvector_result.csv",sep=""),sep=",",header=TRUE)
rlvec$Type = as.factor(apply(rlvec,1,function(x) benchmark_type(x)))
rlvec$ConstructionTime <- as.numeric(as.character(rlvec$ConstructionTime))
rlvec$SpaceBitsPerElement <- as.numeric(as.character(rlvec$SpaceBitsPerElement))
rlvec$RandomAccessTimePerElement <- as.numeric(as.character(rlvec$RandomAccessTimePerElement))
rlvec$SequentialAccessTimePerElement <- as.numeric(as.character(rlvec$SequentialAccessTimePerElement))

#Used for sample experiments
rlvec$Vector <- factor(rlvec$Vector, levels = levels(rlvec$Vector)[c(1,2,5,7,9,4,6,8,3)])

#Plots summarizing stats for all benchmarks
plot(vector_space_plot(rlvec) + facet_wrap(~ Benchmark, scales="free"))
plot(vector_access_time_plot(rlvec) + facet_wrap(~ Benchmark, scales="free"))
plot(time_space_tradeoff_plot_benchmark(rlvec,title="Access Time-Space-Tradeoff per benchmark") + 
       facet_wrap(~ Benchmark, scales="free"))

#Plots summarizing stats for all benchmark types
rlvec_type <-  ddply(rlvec, c("Vector","Type"), aggreg)
plot(vector_space_plot(rlvec_type) + facet_wrap(~ Type))
plot(vector_access_time_plot(rlvec_type) + facet_wrap(~ Type))
plot(time_space_tradeoff_plot_benchmark(rlvec_type,
                                        title="Access Time-Space-Tradeoff per benchmark type",
                                        point_size=5) + facet_wrap(~ Type))

#Plots summarizing stats for all benchmark
rlvec_all <- ddply(rlvec, c("Vector"), aggreg)
plot(vector_space_plot(rlvec_all))
plot(vector_access_time_plot(rlvec_all))
plot(time_space_tradeoff_plot_benchmark(rlvec_all,
                                        title="Access Time-Space-Tradeoff for all benchmarks", point_size=8))

