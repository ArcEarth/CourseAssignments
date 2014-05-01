CREATE TABLE [dbo].[Flights]
(
    [Name] NCHAR(10) NOT NULL, 
    [SourceCity] NCHAR(10) NOT NULL, 
    [TargetCity] NCHAR(10) NOT NULL, 
    [TakeoffTime] TIME NULL, 
    [LandingTime] TIME NULL, 
    CONSTRAINT [FK_SourceCity_To_Cities] FOREIGN KEY ([SourceCity]) REFERENCES [Cities]([Name]), 
    CONSTRAINT [FK_TargetCity_To_Cities] FOREIGN KEY ([TargetCity]) REFERENCES [Cities]([Name]),
    PRIMARY KEY ([Name]), 
)
