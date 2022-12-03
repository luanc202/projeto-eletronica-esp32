import { Button as ButtonNativeBase, IButtonProps, Text } from 'native-base';

interface Props extends IButtonProps {
    title: string;
    textColor: string;
}

export function Button({ title, textColor, ...rest }: Props) {
    return (
        <ButtonNativeBase
            w="full"
            h={14}
            rounded="sm"
            fontSize="md"
            _loading={{
                _spinner: { color: "black" }
            }}
            {...rest}
        >
            <Text
                textTransform='uppercase'
                fontSize="md"
                fontFamily="bold"
                color={textColor}
            >
                {title}
            </Text>
        </ButtonNativeBase >
    )
};