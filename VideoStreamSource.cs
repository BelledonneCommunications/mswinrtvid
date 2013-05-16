using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Windows.Media;

namespace mswp8vid
{
    public class VideoStreamSource : MediaStreamSource, IDisposable
    {
        public class Sample
        {
            public Sample(Windows.Storage.Streams.IBuffer pBuffer, UInt64 hnsPresentationTime)
            {
                this.buffer = pBuffer;
                this.presentationTime = hnsPresentationTime;
            }

            public Windows.Storage.Streams.IBuffer buffer;
            public UInt64 presentationTime;
        }


        public VideoStreamSource(int frameWidth, int frameHeight)
        {
            this.frameWidth = frameWidth;
            this.frameHeight = frameHeight;
            this.sampleQueue = new Queue<Sample>(VideoStreamSource.maxSampleQueueSize);
            this.outstandingSamplesCount = 0;
            mswp8vid.Globals.Instance.VideoSampleDispatcher.sampleReceived += OnSampleReceived;
        }

        #region Implementation of the IDisposable interface
        public void Dispose()
        {
            if (!this.isDisposed)
            {
                mswp8vid.Globals.Instance.VideoSampleDispatcher.sampleReceived -= OnSampleReceived;
                this.isDisposed = true;
            }
            GC.SuppressFinalize(this);
        }
        #endregion

        #region Implementation of the MediaStreamSource interface
        protected override void OpenMediaAsync()
        {
            CreateMediaDescription();

            // Create attributes telling that we want an infinite video in which we can't seek
            Dictionary<MediaSourceAttributesKeys, string> sourceAttributes = new Dictionary<MediaSourceAttributesKeys, string>();
            sourceAttributes[MediaSourceAttributesKeys.Duration] = TimeSpan.FromSeconds(0).Ticks.ToString(CultureInfo.InvariantCulture);
            sourceAttributes[MediaSourceAttributesKeys.CanSeek] = false.ToString();

            List<MediaStreamDescription> streams = new List<MediaStreamDescription>();
            streams.Add(this.streamDesc);
            ReportOpenMediaCompleted(sourceAttributes, streams);
        }

        protected override void GetSampleAsync(MediaStreamType mediaStreamType)
        {
            System.Diagnostics.Debug.WriteLine("GetSampleAsync " + mediaStreamType.ToString());
            if (mediaStreamType == MediaStreamType.Video)
            {
                lock (lockObj)
                {
                    this.outstandingSamplesCount++;
                    FeedSamples();
                }
            }
        }

        protected override void CloseMedia()
        {
        }

        protected override void GetDiagnosticAsync(MediaStreamSourceDiagnosticKind diagnosticKind)
        {
            throw new NotImplementedException();
        }

        protected override void SwitchMediaStreamAsync(MediaStreamDescription mediaStreamDescription)
        {
            throw new NotImplementedException();
        }

        protected override void SeekAsync(long seekToTime)
        {
            ReportSeekCompleted(seekToTime);
        }
        #endregion

        public void Shutdown()
        {
            // TODO
        }

        private void OnSampleReceived(Windows.Storage.Streams.IBuffer pBuffer, UInt64 hnsPresentationTime)
        {
            lock (lockObj)
            {
                if (this.sampleQueue.Count >= VideoStreamSource.maxSampleQueueSize)
                {
                    // The sample queue is full, discard the older sample
                    this.sampleQueue.Dequeue();
                }

                this.sampleQueue.Enqueue(new Sample(pBuffer, hnsPresentationTime));
                FeedSamples();
            }
        }

        private void CreateMediaDescription()
        {
            Dictionary<MediaStreamAttributeKeys, string> streamAttributes = new Dictionary<MediaStreamAttributeKeys, string>();
            streamAttributes[MediaStreamAttributeKeys.VideoFourCC] = "H264";
            streamAttributes[MediaStreamAttributeKeys.Width] = this.frameWidth.ToString();
            streamAttributes[MediaStreamAttributeKeys.Height] = this.frameHeight.ToString();
            this.streamDesc = new MediaStreamDescription(MediaStreamType.Video, streamAttributes);
        }

        private void FeedSamples()
        {
            while (this.sampleQueue.Count > 0 && this.outstandingSamplesCount > 0)
            {
                Sample sample = this.sampleQueue.Dequeue();
                Stream s = System.Runtime.InteropServices.WindowsRuntime.WindowsRuntimeBufferExtensions.AsStream(sample.buffer);

                MediaStreamSample msSample = new MediaStreamSample(this.streamDesc, s, 0, s.Length, (long)sample.presentationTime, this.emptyAttributes);
                ReportGetSampleCompleted(msSample);
                this.outstandingSamplesCount--;
            }
        }


        private const int maxSampleQueueSize = 4;

        private bool isDisposed = false;
        private int frameWidth;
        private int frameHeight;
        private int outstandingSamplesCount;
        private object lockObj = new object();
        private Queue<Sample> sampleQueue;
        private MediaStreamDescription streamDesc;
        private Dictionary<MediaSampleAttributeKeys, string> emptyAttributes = new Dictionary<MediaSampleAttributeKeys, string>();
    }
}
